function showDesiredResult

  kf = setupKalman(.0005, 1);
  
  [run, measurement] = getNextMeasurement;
  
  while( run )
    actualMeas = (measurement+50)*.0002;
    %Add noise that is constant
    addedNoise = 2*(rand-.5)*.002;
      
    myMeas = actualMeas + addedNoise;
    %"Special" noise covariance
    %kf = scaleMeasurementNoise(kf,9*myMeas^4);
    %"Normal" noise covariance
    kf = scaleMeasurementNoise(kf,.001);
    kf = runSIKalman( kf, myMeas, 0 );
    data.filtered(kf.iteration) = kf.ph_kp1gkp1(1,1);
    data.unfiltered(kf.iteration) = myMeas;
    data.actual(kf.iteration) = actualMeas;
    [run, measurement] = getNextMeasurement(run, measurement);    
  end
  
  min(data.unfiltered)
  
  hold off;
  plot( data.unfiltered, 'b.');
  hold on;
  plot( data.filtered, 'r-', 'LineWidth', 3.0);
  plot( data.actual, 'k-', 'LineWidth', 1.0);
  legend( 'estimation', 'measurement' );
  xlabel( 'frame' );
  ylabel( 'position' );
  title( 'Desired Result' );
  
  figure
  plot( abs(data.filtered-data.actual) ./ data.actual .*100 );
  title( '% error');
  
end