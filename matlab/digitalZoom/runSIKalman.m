function kf=runSIKalman( kf, x, controlX )

  %Set the measurement
  kf.m_kp1(1,1) = x;
  %and control inputs
  kf.u_k(1,1) = controlX;
  %Set the measurement transition matrix
  kf.H_kp1(1,1) = 1;
      
  if( kf.iteration == 0 )
    %Save initial position    
    kf.ph_kp1gkp1(1,1) = x;
  else
    if( kf.iteration == 1 )  
      %Save intitial position and velocity. 
      %Now initial conditions are set.
      kf.ph_kp1gkp1(2,1) = x - kf.ph_kp1gkp1(1,1);
      kf.ph_kp1gkp1(1,1) = x;
      kf = nextIteration(kf);
      kf = prediction(kf);
      
    end

    kf=measurement(kf);
    kf=correction(kf);
    kf=nextIteration(kf);
    kf=prediction(kf);
  end
  kf.iteration = kf.iteration + 1;
end

