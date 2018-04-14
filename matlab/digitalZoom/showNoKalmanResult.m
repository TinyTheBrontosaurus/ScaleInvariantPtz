function runSimulation

  kf = setupKalman(.0005, 1);
  
  [run, measurement] = getNextMeasurement;
  
  while( run )
    actualMeas = (measurement+35)*.0005;
    %Add noise that varies by size of measurement
    addedNoise = 2*(rand-.5)*3*actualMeas^2;
      
    myMeas = actualMeas + addedNoise;
    %"Special" noise covariance
    %kf = scaleMeasurementNoise(kf,(9*myMeas^4));
    %kf = scaleProcessNoise(kf,(3*myMeas)^1);
    %"Normal" noise covariance
    %kf = scaleMeasurementNoise(kf,.001);
    kf = runSIKalman( kf, myMeas, 0 );
    data.filtered(kf.iteration) = myMeas;
    data.unfiltered(kf.iteration) = myMeas;
    data.actual(kf.iteration) = actualMeas;
    [run, measurement] = getNextMeasurement(run, measurement);    
  end
  
  figure('Name', 'Unfiltered Result' );
  hold off;
  plot( data.unfiltered, 'b.');
  hold on;
  plot( data.filtered, 'r-', 'LineWidth', 3.0);
  plot( data.actual, 'k-', 'LineWidth', 1.0);
  legend( 'estimation', 'measurement' );
  xlabel( 'frame' );
  ylabel( 'position' );
  title( 'Unfiltered Result' );
  
  figure('Name', 'Unfiltered % error' );
  
  plot( abs(data.unfiltered-data.actual) ./ data.actual .*100 );
  xlabel( 'frame' );
  ylabel( '% error' );
  title( 'Unfiltered % error');
  
end