function kf=setupKalman(processNoise, measurementNoise)
  r=1;
  kf.R = r*r;
  kf.Q = [1e-6/3 1e-6/2; 1e-6/2 1e-6];
  
  kf.Q = kf.Q * (processNoise/0.03);
  kf.Q_k = kf.Q;
  
  %Set the measurement noise
  kf.R = measurementNoise * measurementNoise;
  kf.R_kp1 = kf.R;
  
  %Set the initial state covariance
  kf.Ph_kp1gkp1 = [0.5 0.5; 0.5 0.5];
  
  %Set the state transition matrix
  kf.F_k = [1 1; 0 1];
  %Set the measurement transition matrix
  kf.H_kp1 = [1 0];
  
  kf.iteration = 0;
  
end