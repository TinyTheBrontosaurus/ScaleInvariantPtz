function [run, measurement] = ...
    getNextMeasurement(run, measurement)

  if( nargin == 0 )
    run = 900;
    measurement = 0;
    
  elseif( nargin ~= 2 )
    error( 'getNextMeasurement takes 2 args or none');
  else
    %constant velocity up
    if( run > 800 )
      measurement = 900 - run;
    %Accel down
    elseif( run > 700)
       measurement = (800-run)-0.01*(800-run)*(800-run)+100; 
    %constant velocity down
    elseif( run > 600 )
      measurement = run-600;
    %Accel up
    elseif( run > 500 )
      measurement = run-600 + 0.01*(run-600)*(run-600);   
    %constant velocity up
    elseif( run > 400 )
      measurement = 500 - run;
    else
      run = 1;
    end   
    run=run-1;
  end
  