#include "laserPeopleCommon.h"

CLaserPeopleCommon::CLaserPeopleCommon()
{
  initialize();
  debug           = false;
  jump_distance   = 0.1;
  min_points      = 3;
}

CLaserPeopleCommon::~CLaserPeopleCommon()
{
}

void CLaserPeopleCommon::initialize()
{
  ranges.clear();
  points.clear();
  segments.clear();
  features.clear();
}

void CLaserPeopleCommon::extractPoints()
{
  if(debug)
    std::cout << "CLaserPeopleCommon::extractPoints()" << std::endl;

  float scanangle = angle_max - angle_min;         // rad
  float scanpoints = scanangle / angle_increment;  // number of points
  // Polars to cartesian
  for(unsigned int i = 0; i<ranges.size(); i++)
  {
    float th =  scanangle/(scanpoints-1)*i + angle_min;
    if(ranges[i] >= 0.02 && !isnan(ranges[i]) && !isinf(ranges[i])) // metres
    {
      std::pair<double,double> point;
      point.first  = ranges[i] * cos(th);
      point.second = ranges[i] * sin(th);
      points.push_back(point);
    }
    else
    {
      //rejected point
      /*
      std::pair<double,double> point;
      point.first  = 60.0 * cos(th); //far
      point.second = 60.0 * sin(th);
      points.push_back(point);
      */
    }
  }

  if(debug)
    std::cout << "points.size(): "<<points.size() << std::endl;
}

void CLaserPeopleCommon::extractSegments()
{
  if(debug)
    std::cout << "CLaserPeopleCommon::extractSegments()" << std::endl;

  float jd_sq = jump_distance*jump_distance;
  int cur_seg_start = 1;
  for(unsigned int i=0+1; i<points.size();i++)
  {
    double x = points[i].first;
    double y = points[i].second;
    double x0 = points[i-1].first;
    double y0 = points[i-1].second;
    if( ( x!=0 && y!=0 ) && pow(x-x0,2) + pow(y-y0,2) > jd_sq )
    {
      if(i-cur_seg_start >= min_points - 1) // number of points >= 3
      {
        std::pair<int,int> seg;
        seg.first  = cur_seg_start;
        seg.second = i;
        segments.push_back(seg);
      }
      cur_seg_start = i+1;
    }
  }
  // Close last segment
  if(points.size() - cur_seg_start >= min_points - 1) // number of points >= 3
  {
    std::pair<int,int> seg;
    seg.first  = cur_seg_start;
    seg.second = points.size();
    segments.push_back(seg);
  }

  if(debug)
    std::cout << "segments.size(): " << segments.size() << std::endl;
}

void CLaserPeopleCommon::extractFeatures()
{
  if(debug)
    std::cout << "CLaserPeopleCommon::extractFeatures()" << std::endl;

  unsigned int np;
  int middle,stot,spre,spost;
  double stdev,stdev_med,width,sumxx,sumxy,sumyy,ssxx,ssyy,ssxy;
  double r,radius,sc,boundary,mean_d,stdev_d,k,d1,d2,d3,sp,area,mean_k;
  double beta,alpha1,alpha2,mean_beta,jd_prec,jd_suc,jd_min,jd_max;
  double pairwidth,distpair,iniciox,inicioy,finalx,finaly;
  int t=0;

  for(int s=0; s<(int)segments.size(); s++)
  {
    int pi = segments[s].first;
    int pf = segments[s].second;
    np = pf - pi + 1;
    if(np<min_points)
    {
      continue; //segments with less than 3 points, not included
    }
    //1. number of points (np)
    //np;

    //2. standard deviation (stdev)
    std::vector<double> sum(2,0);
    std::vector<double> mean(2,0);

    for(int m=pi-1; m<pf; m++)
    {
      sum[0] += points[m].first;
      sum[1] += points[m].second;
    }
    mean[0]=sum[0]/np;
    mean[1]=sum[1]/np;
    stdev=0;
    for(int m=pi-1; m<pf; m++)
    {
      stdev+= pow((points[m].first - mean[0]),2) + pow((points[m].second - mean[1]),2);
    }
    stdev=sqrt(stdev/(np-1)); //TODO:np?

    // 3.mean average deviation from median (stdev_med)
    std::vector<double> scan_sx, scan_sy;
    for(int m = pi - 1; m<pf; m++)
    {
      scan_sx.push_back(points[m].first);
      scan_sy.push_back(points[m].second);
    }

    sort(scan_sx.begin(), scan_sx.end());
    sort(scan_sy.begin(), scan_sy.end());

    middle = scan_sx.size()/2.0;
    std::vector<double> median(2,0);

    if (!(scan_sx.size() % 2))
    {
     median[0]=(scan_sx[middle]+scan_sx[middle-1])/2.0;
     median[1]=(scan_sy[middle]+scan_sy[middle-1])/2.0;
    }
    else
    {
      median[0]=scan_sx[middle];
      median[1]=scan_sy[middle];
    }

    stdev_med=0;
    for(int m=pi-1 ; m<pf ; m++ )
    {
      stdev_med+= sqrt(pow((points[m].first-median[0]),2) + pow((points[m].second-median[1]),2)) ;
    }
    stdev_med=stdev_med/np;

    //4. after feature nº13.
    //5. after feature nº13.

    // 6.width (distance between first and last points of a segment) (width)
    width = sqrt( pow((points[pi - 1].first - points[pf - 1].first),2) + pow(( points[pi - 1].second - points[pf - 1].second ),2) );

    // 7.linearity (r)
    // least suqares line fitting as per wolfram's
    sumxx=0;
    sumxy=0;
    sumyy=0;
    for(int m=pi-1 ; m<pf ; m++ )
    {
      sumxx+=pow(points[m].first,2);
      sumxy+=(points[m].first)*(points[m].second);
      sumyy+=pow(points[m].second,2);
    }
    //float denom = sumxx - np*pow(mean(0),2);
    // y=a+b*x
    //float a = (mean(1)*sumxx - mean(0)*sumxy) / denom;
    //float b = (sumxy - np*mean(0)*mean(1)) / denom;

    // correlation coefficient
    ssxx = sumxx - np*pow(mean[0],2);
    ssyy = sumyy - np*pow(mean[1],2);
    ssxy = sumxy - np*mean[0]*mean[1];
    r = fabs(ssxy) / sqrt(ssxx*ssyy);
    if(r>1000 || (r != r))
    {
      r=0;
    }

    // 8.circularity (sc)
    sc     = 0.0;
    radius = 0.0;


    scan_sx.clear();
    scan_sy.clear();
    for(int m = pi - 1; m<pf; m++)
    {
      scan_sx.push_back(points[m].first);
      scan_sy.push_back(points[m].second);
    }

    //EIGEN
    //Copy segment x and y in Eigen vectors (escan_sx, escan_sy)
    Eigen::VectorXf escan_sx(scan_sx.size());
    escan_sx.setConstant(0);
    Eigen::VectorXf escan_sy(scan_sy.size());
    escan_sy.setConstant(0);

    for(unsigned int u=0; u<scan_sx.size(); u++)
    {
      escan_sx(u)=float(scan_sx[u]);
      escan_sy(u)=float(scan_sy[u]);
    }

    Eigen::MatrixXf AA(escan_sx.size(), 3);
    AA.col(0)=2*escan_sx.transpose();
    AA.col(1)=2*escan_sy.transpose();
    AA.col(2).setConstant(1);

    Eigen::MatrixXf bb(escan_sx.cols(), 1);
    bb = -(escan_sx.array() * escan_sx.array() + escan_sy.array() * escan_sy.array()).matrix();
    Eigen::Vector3f res(0,0,0);
    Eigen::FullPivLU<Eigen::MatrixXf> lu_decomp(AA);

    if(lu_decomp.rank()==3)
    {
      res = ((AA.transpose()*AA).inverse())*(AA.transpose())*bb;
    }

    Eigen::Vector2f center(-res(0),-res(1));
    radius = sqrt(center(0)*center(0)+center(1)*center(1)-res(2));
    if(radius!=radius)
      radius=0;

    sc=0;
    for(int m=0;m<escan_sx.cols();m++)
    {
      sc += pow(radius - sqrt(pow( escan_sx(m) - center(0) ,2)+pow( escan_sy(m) - center(1) ,2)),2);
    }

    if(sc>1000 || (sc != sc))
    {
      sc=0;
    }

    // 9. radius (of the circle fitted to the segment; see above) (radius)
    // radius;

    // 10. boundary lenght (boundary)
    scan_sx.clear();
    scan_sy.clear();
    for(int m = pi - 1; m<pf; m++)
    {
      scan_sx.push_back(points[m].first);
      scan_sy.push_back(points[m].second);
    }

    boundary=0;
    std::vector<double> d(scan_sx.size()-1);

    for(unsigned int m=1;m<scan_sx.size();m++)
    {
      d[m-1]= sqrt(pow( scan_sx[m] - scan_sx[m-1] ,2)+pow( scan_sy[m] - scan_sy[m-1] ,2));
      boundary += d[m-1];
    }

    // 11. boundary regularity (stdev_d)
    mean_d = boundary / d.size();
    stdev_d = 0;
    for(unsigned int m=0; m<d.size(); m++)
    {
      stdev_d += pow(d[m]-mean_d,2);
    }
    stdev_d = sqrt( stdev_d/(d.size()-1) );

    // 12. mean curvature (mean_k)
    k=0;
    for(unsigned int m=1; m<scan_sx.size()-1; m++)
    {
      d1   = sqrt(pow((scan_sx[m]  -scan_sx[m-1]),2)+pow((scan_sy[m]  -scan_sy[m-1]),2));
      d2   = sqrt(pow((scan_sx[m]  -scan_sx[m+1]),2)+pow((scan_sy[m]  -scan_sy[m+1]),2));
      d3   = sqrt(pow((scan_sx[m-1]-scan_sx[m+1]),2)+pow((scan_sy[m-1]-scan_sy[m+1]),2));
      sp   = ( d1 + d2 + d3 ) / 2; //semiperimeter
      area = sqrt(sp*(sp - d1)*(sp - d2)*(sp - d3)); //heron's formula
      k   += 4*area/(d1*d2*d3);
    }
    mean_k = k / (np - 2);

    if(mean_k>1000 || (mean_k != mean_k))
    {
      mean_k=0;
    }

    // 13. mean angular difference (mean_beta)
    beta=0;
    for(unsigned int m=1 ; m<scan_sx.size()-1 ; m++ )
    {
      alpha1 = atan2( scan_sy[m]   - scan_sy[m+1], scan_sx[m]   - scan_sx[m+1]);
      alpha2 = atan2( scan_sy[m-1] - scan_sy[m]  , scan_sx[m-1] - scan_sx[m]  );
      beta  += alpha1 - alpha2;
    }
    mean_beta = beta / (np - 2);

    // 4 & 5. distances to prec and suc segments (jd_min, jd_max)
    jd_prec=0;    jd_suc=0;    jd_min=0;    jd_max=0;    pairwidth = 0;
    stot=-1;    spre= -1;    spost = -1;

    std::vector<double>::iterator spre_iter;
    std::vector<double>::iterator spost_iter;

    std::vector<double> distapre(segments.size(), 10);
    std::vector<double> distapost(segments.size(), 10);

    if(segments.size()>1) //more than 1 segment
    {
      if(s>0)
      {
          for(int m=0;m<s;m++)
        {
          int mi = segments[m].first;
          int mf = segments[m].second;
          if(mf-mi+1 >=3)
          {
            distapre[m]= sqrt(pow((points[mf - 1].first - points[pi - 1].first ),2) + pow((points[mf - 1].second - points[pi - 1].second ),2)) ;
          }
        }

        spre_iter = min_element(distapre.begin(), distapre.end());
        jd_prec = *spre_iter;
        spre = distance(distapre.begin(), spre_iter);
        //spre=int(spre_iter);
      }
      else
      {
        jd_prec= rand() % 4 + 0.51;
      }

      if(s<(int)segments.size()-1)
      {
        for(unsigned int m=s+1;m<segments.size();m++)
        {
          int mi = segments[m].first;
          int mf = segments[m].second;
          if(mf-mi+1 >=3)
          {
            distapost[m]= sqrt(pow((points[pf-1].first-points[mi-1].first),2)+pow((points[pf-1].second-points[mi-1].second),2)) ;
          }
        }

        spost_iter = min_element(distapost.begin(), distapost.end());
        jd_suc = *spost_iter;
        spost = distance(distapost.begin(), spost_iter);
        //spost=int(spost_iter);
      }
      else
      {
        jd_suc=rand() % 4 + 0.51;
      }
    }
    jd_min=std::min(jd_prec,jd_suc);
    jd_max=std::max(jd_prec,jd_suc);

    //14. width of pair with nearest segment (pairwidth)
    distpair=3; //limit distance to make pairs of segments
    if(jd_prec>0 && jd_suc>0 && jd_min<distpair)
    {
      if(jd_prec<=jd_suc)
      {
        stot=spre;
      }
      else
      {
        stot=spost;
      }
    }
    else if(jd_prec>0 && jd_prec<distpair)
    {
      stot=spre;
    }
    else if(jd_suc>0 && jd_suc<distpair)
    {
      stot=spost;
    }
    else
    {
      stot=-1;
    }

    if(stot>-1) // stot, nearest segment
    {
      if(stot<s)
      {
        k=stot;
      }
      else if (stot>s)
      {
        k=s;
      }

      iniciox = points[segments[k].first -1].first;
      inicioy = points[segments[k].first -1].second;

      if(stot<s)
      {
        k=s;
      }
      else if (stot>s)
      {
        k=stot;
      }
      finalx = points[segments[k].second -1].first;
      finaly = points[segments[k].second -1].second;
      pairwidth = sqrt(pow((finalx-iniciox),2)+pow((finaly-inicioy),2) ); //euclidian distance between bound points
    }
    else
    {
     pairwidth = width;
    }

    // build features array

    std::vector<double> features_s(NUMBER_OF_FEATURES);

    features_s[0]  = np;
    features_s[1]  = stdev;
    features_s[2]  = stdev_med;
    features_s[3]  = jd_min;
    features_s[4]  = jd_max;
    features_s[5]  = width;
    features_s[6]  = r;
    features_s[7]  = sc;
    features_s[8]  = radius;
    features_s[9]  = boundary;
    features_s[10] = stdev_d;
    features_s[11] = mean_k;
    features_s[12] = mean_beta;
    features_s[13] = pairwidth;

    features.push_back(features_s);

    t++;
  }
  if(debug)
    std::cout << "features.size(): " << features.size() << std::endl;
  if(debug)
    std::cout << "features[0].size(): " << features[0].size() << std::endl;

}

void CLaserPeopleCommon::scanIteration(const std::vector<float> inRanges, const float angleMin, const float angleMax, const float angleIncrement)
{
  if(debug)
    std::cout << "CLaserPeopleCommon::scanIteration()" << std::endl;

  initialize();
  angle_min       = angleMin;
  angle_max       = angleMax;
  angle_increment = angleIncrement;
  ranges = inRanges;
  extractPoints();
  extractSegments();
  extractFeatures();
}

void CLaserPeopleCommon::scanIteration2(const std::vector<float> inRanges, const float angleMin, const float angleMax, const float angleIncrement)
{
  if(debug)
    std::cout << "CLaserPeopleCommon::scanIteration()" << std::endl;

  initialize();
  angle_min       = angleMin;
  angle_max       = angleMax;
  angle_increment = angleIncrement;
  ranges = inRanges;
  extractPoints();
  extractSegments();
  //extractFeatures();

}

void CLaserPeopleCommon::setJumpDistance(const float jumpDistance)
{
  if(debug)
    std::cout << "CLaserPeopleCommon::setJumpDistance" << std::endl;
  if(jump_distance!=jumpDistance)
  {
    jump_distance = jumpDistance;
    std::cout << "CLaserPeopleCommon::setJumpDistance: jump distance set to: "<<jump_distance << std::endl;
  }
}

void CLaserPeopleCommon::setMinPoints(const unsigned int minPoints)
{
  if(debug)
    std::cout << "CLaserPeopleCommon::setMinPoints" << std::endl;
  if(min_points!=minPoints)
  {
    min_points = minPoints;
    std::cout << "CLaserPeopleCommon::setMinPoints: min points set to: "<< min_points << std::endl;
  }
}

float CLaserPeopleCommon::getSegmentDeviation(const unsigned int segment)
{
  if(features.size()>=segment)
    return features[segment][1];
  else
    return -1.0;
}
