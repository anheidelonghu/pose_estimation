#include "pose_estimation.h"
#include "test.h"


static void showMatchImage(cv::Mat& img_1, cv::Mat& img_2, vector<KeyPoint>& keyPoint_1, vector<KeyPoint>& keyPoint_2, vector<DMatch>& matches)
{
  cv::Mat showImg;
  drawMatches(img_1,keyPoint_1,img_2,keyPoint_2,matches,showImg);
  namedWindow("show match",0);
  imshow("show match",showImg);
  waitKey(0);
}

static int img_assemble(cv::Mat& rgb, cv::Mat& depth, deque <cv::Mat>& twoFrames)
{
  if(twoFrames.size() == 2)
  {
    twoFrames.pop_front();
    twoFrames.push_back(rgb);
    twoFrames.pop_front();
    twoFrames.push_back(depth);
  }
  else if(twoFrames.size() == 0)
  {
    twoFrames.push_back(rgb);
    twoFrames.push_back(depth);
  }
  else{
    cout << "deque size is incorrect!" << endl;
    return 4;
  }
}

//calculate R and t between to frames
int getMotion(cv::Mat& rgb_stream, cv::Mat& depth_stream, 
	      deque <cv::Mat>& twoFrames, 
	      cv::Mat& R, cv::Mat& t, 
	      bool showMatch, 
	      featureType type)
{
  //get picture stream
  cv::Mat _rgb_stream = rgb_stream.clone();
  cv::Mat _depth_stream = depth_stream.clone();
  //cout << rgb_stream.depth() << " & " << depth_stream.depth() << endl;
  if(_rgb_stream.empty() || _depth_stream.empty())
  {
    cout << "rgb stream or depth stream is empty!" << endl;
    if(img_assemble(_rgb_stream, _depth_stream, twoFrames) == 4) return 4;
    return 1;
  }                                                     
  
  //if deque is full, begin to calculate pose,
  if(twoFrames.size() == 2)
  {
    cv::Mat img_1 = twoFrames.at(0);	//last image
    cv::Mat img_2 = _rgb_stream;	//image now
    cv::Mat depth1 = twoFrames.at(1);
    cv::Mat depth2 = _depth_stream;
    vector <KeyPoint> keypoints_1, keypoints_2;
    vector <DMatch> matches;
    switch(type)
    {
      case ORBType:
      {
	find_feature_matches( img_1, img_2, keypoints_1, keypoints_2, matches);
	break;
      }
      case SURFType:
      {
	find_feature_matches_surf( img_1, img_2, keypoints_1, keypoints_2, matches);
	break;
      }
    }
    
    if(matches.size() < 5)
    {
//       imwrite("errorbox/rgb1.jpg",img_1);
//       imwrite("errorbox/rgb2.jpg",img_2);
//       imwrite("errorbox/depth1.jpg",depth1);
//       imwrite("errorbox/depth2.jpg",depth2);
      
      cout << "matches size: " << matches.size() << endl;
      if(img_assemble(_rgb_stream, _depth_stream, twoFrames) == 4) return 4;
      return 2;
    }
    cout<<"一共找到了"<<matches.size() <<"组匹配点"<<endl;
    
    if(showMatch) showMatchImage(img_1,img_2,keypoints_1,keypoints_2,matches);
    
    Mat K = ( Mat_<double> ( 3,3 ) << 309.414, 0, 167.38, 0, 312.495, 117.689, 0, 0, 1 );		//intrinsic matrix
    vector<Point3f> pts1, pts2;
    
    for ( DMatch m:matches )
    {
        ushort d1 = depth1.ptr<unsigned short> ( int ( keypoints_1[m.queryIdx].pt.y ) ) [ int ( keypoints_1[m.queryIdx].pt.x ) ];
        ushort d2 = depth2.ptr<unsigned short> ( int ( keypoints_2[m.trainIdx].pt.y ) ) [ int ( keypoints_2[m.trainIdx].pt.x ) ];
        if ( d1==0 || d2==0 )   // bad depth
            continue;
        Point2d p1 = pixel2cam ( keypoints_1[m.queryIdx].pt, K );
        Point2d p2 = pixel2cam ( keypoints_2[m.trainIdx].pt, K );
        float dd1 = float ( d1 ) /1000.0;
        float dd2 = float ( d2 ) /1000.0;
        pts1.push_back ( Point3f ( p1.x*dd1, p1.y*dd1, dd1 ) );
        pts2.push_back ( Point3f ( p2.x*dd2, p2.y*dd2, dd2 ) );
    }
    
    if(pts1.size() < 5)
    {
      cout << "pts1 size: " << pts1.size() << endl;
      if(img_assemble(_rgb_stream, _depth_stream, twoFrames) == 4) return 4;
      return 3;
    }
    //cout << "pts size: " << pts1.size() << endl;
    
    cout<<"3d-3d pairs: "<<pts1.size() <<endl;
    Mat R, t;
    pose_estimation_3d3d ( pts1, pts2, R, t );
    cout<<"ICP via SVD results: "<<endl;
    cout<<"R = "<<R<<endl;
    cout<<"t = "<<t<<endl;
    cout<<"R_inv = "<<R.t() <<endl;
    cout<<"t_inv = "<<-R.t() *t<<endl;

    cout<<"calling bundle adjustment"<<endl;

    bundleAdjustment( pts1, pts2, R, t );
    
    // verify p1 = R*p2 + t
    for ( int i=0; i<5; i++ )
    {
        cout<<"p1 = "<<pts1[i]<<endl;
        cout<<"p2 = "<<pts2[i]<<endl;
        cout<<"(R*p2+t) = "<< 
            R * (Mat_<double>(3,1)<<pts2[i].x, pts2[i].y, pts2[i].z) + t
            <<endl;
        cout<<endl;
    }
    
  }
  
  
  //push to Mat deque
  if(img_assemble(_rgb_stream, _depth_stream, twoFrames) == 4) return 4;

}


void find_feature_matches ( const Mat& img_1, const Mat& img_2,
                            std::vector<KeyPoint>& keypoints_1,
                            std::vector<KeyPoint>& keypoints_2,
                            std::vector< DMatch >& matches )
{
    //-- frequency
    int64 time0 = getTickCount();
    //-- 初始化
    Mat descriptors_1, descriptors_2;
    // used in OpenCV3 
    Ptr<FeatureDetector> detector = ORB::create();
    Ptr<DescriptorExtractor> descriptor = ORB::create();
    // use this if you are in OpenCV2 
    // Ptr<FeatureDetector> detector = FeatureDetector::create ( "ORB" );
    // Ptr<DescriptorExtractor> descriptor = DescriptorExtractor::create ( "ORB" );
    Ptr<DescriptorMatcher> matcher  = DescriptorMatcher::create("BruteForce-Hamming");
    //-- 第一步:检测 Oriented FAST 角点位置
    detector->detect ( img_1,keypoints_1 );
    detector->detect ( img_2,keypoints_2 );

    //-- 第二步:根据角点位置计算 BRIEF 描述子
    descriptor->compute ( img_1, keypoints_1, descriptors_1 );
    descriptor->compute ( img_2, keypoints_2, descriptors_2 );

    //-- 第三步:对两幅图像中的BRIEF描述子进行匹配，使用 Hamming 距离
    vector<DMatch> match;
   // BFMatcher matcher ( NORM_HAMMING );
    matcher->match ( descriptors_1, descriptors_2, match );

    //-- 第四步:匹配点对筛选
    double min_dist=10000, max_dist=0;

    //找出所有匹配之间的最小距离和最大距离, 即是最相似的和最不相似的两组点之间的距离
    for ( int i = 0; i < descriptors_1.rows; i++ )
    {
        double dist = match[i].distance;
        if ( dist < min_dist ) min_dist = dist;
        if ( dist > max_dist ) max_dist = dist;
    }

    printf ( "-- Max dist : %f \n", max_dist );
    printf ( "-- Min dist : %f \n", min_dist );

    //当描述子之间的距离大于两倍的最小距离时,即认为匹配有误.但有时候最小距离会非常小,设置一个经验值30作为下限.
    for ( int i = 0; i < descriptors_1.rows; i++ )
    {
        if ( match[i].distance <= max ( 2*min_dist, 30.0 ) )
        {
            matches.push_back ( match[i] );
        }
    }
    
    //output frequency
    cout << "当前帧率为： " << getTickFrequency() / (getTickCount() - time0) << endl;
}

Point2d pixel2cam ( const Point2d& p, const Mat& K )
{
    return Point2d
           (
               ( p.x - K.at<double> ( 0,2 ) ) / K.at<double> ( 0,0 ),
               ( p.y - K.at<double> ( 1,2 ) ) / K.at<double> ( 1,1 )
           );
}

void pose_estimation_3d3d (
    const vector<Point3f>& pts1,
    const vector<Point3f>& pts2,
    Mat& R, Mat& t
)
{
    Point3f p1, p2;     // center of mass
    int N = pts1.size();
    for ( int i=0; i<N; i++ )
    {
        p1 += pts1[i];
        p2 += pts2[i];
    }
    p1 = Point3f( Vec3f(p1) /  N);
    p2 = Point3f( Vec3f(p2) / N);
    vector<Point3f>     q1 ( N ), q2 ( N ); // remove the center
    for ( int i=0; i<N; i++ )
    {
        q1[i] = pts1[i] - p1;
        q2[i] = pts2[i] - p2;
    }

    // compute q1*q2^T
    //Eigen::Matrix3d W = Eigen::Matrix3d::Zero();
    Eigen::Matrix3d W = Eigen::Matrix3d::Zero();
//     W << 0,0,0,
// 	 0,0,0,
// 	 0,0,0;
    for ( int i=0; i<N; i++ )
    {
        W += Eigen::Vector3d ( q1[i].x, q1[i].y, q1[i].z ) * Eigen::Vector3d ( q2[i].x, q2[i].y, q2[i].z ).transpose();
    }
    cout<<"W="<<W<<endl;

    // SVD on W
    Eigen::JacobiSVD<Eigen::Matrix3d> svd ( W, Eigen::ComputeFullU|Eigen::ComputeFullV );
    Eigen::Matrix3d U = svd.matrixU();
    Eigen::Matrix3d V = svd.matrixV();
    cout<<"U="<<U<<endl;
    cout<<"V="<<V<<endl;
    
    Eigen::Matrix3d R_ = U* ( V.transpose() );
    Eigen::Vector3d t_ = Eigen::Vector3d ( p1.x, p1.y, p1.z ) - R_ * Eigen::Vector3d ( p2.x, p2.y, p2.z );

    // convert to cv::Mat
    R = ( Mat_<double> ( 3,3 ) <<
          R_ ( 0,0 ), R_ ( 0,1 ), R_ ( 0,2 ),
          R_ ( 1,0 ), R_ ( 1,1 ), R_ ( 1,2 ),
          R_ ( 2,0 ), R_ ( 2,1 ), R_ ( 2,2 )
        );
    t = ( Mat_<double> ( 3,1 ) << t_ ( 0,0 ), t_ ( 1,0 ), t_ ( 2,0 ) );
}

void bundleAdjustment (
    const vector< Point3f >& pts1,
    const vector< Point3f >& pts2,
    Mat& R, Mat& t )
{
    // 初始化g2o
    typedef g2o::BlockSolver< g2o::BlockSolverTraits<6,3> > Block;  // pose维度为 6, landmark 维度为 3
    Block::LinearSolverType* linearSolver = new g2o::LinearSolverEigen<Block::PoseMatrixType>(); // 线性方程求解器
    Block* solver_ptr = new Block( linearSolver );      // 矩阵块求解器
    g2o::OptimizationAlgorithmGaussNewton* solver = new g2o::OptimizationAlgorithmGaussNewton( solver_ptr );
    g2o::SparseOptimizer optimizer;
    optimizer.setAlgorithm( solver );

    // vertex
    g2o::VertexSE3Expmap* pose = new g2o::VertexSE3Expmap(); // camera pose
    pose->setId(0);
    pose->setEstimate( g2o::SE3Quat(
        Eigen::Matrix3d::Identity(),
        Eigen::Vector3d( 0,0,0 )
    ) );
    optimizer.addVertex( pose );

    // edges
    int index = 1;
    vector<EdgeProjectXYZRGBDPoseOnly*> edges;
    for ( size_t i=0; i<pts1.size(); i++ )
    {
        EdgeProjectXYZRGBDPoseOnly* edge = new EdgeProjectXYZRGBDPoseOnly( 
            Eigen::Vector3d(pts2[i].x, pts2[i].y, pts2[i].z) );
        edge->setId( index );
        edge->setVertex( 0, dynamic_cast<g2o::VertexSE3Expmap*> (pose) );
        edge->setMeasurement( Eigen::Vector3d( 
            pts1[i].x, pts1[i].y, pts1[i].z) );
        edge->setInformation( Eigen::Matrix3d::Identity()*1e4 );
        optimizer.addEdge(edge);
        index++;
        edges.push_back(edge);
    }

    chrono::steady_clock::time_point t1 = chrono::steady_clock::now();
    optimizer.setVerbose( true );
    optimizer.initializeOptimization();
    optimizer.optimize(10);
    chrono::steady_clock::time_point t2 = chrono::steady_clock::now();
    chrono::duration<double> time_used = chrono::duration_cast<chrono::duration<double>>(t2-t1);
    cout<<"optimization costs time: "<<time_used.count()<<" seconds."<<endl;

    cout<<endl<<"after optimization:"<<endl;
    cout<<"T="<<endl<<Eigen::Isometry3d( pose->estimate() ).matrix()<<endl;
    
}
