function kf=nextIteration(kf)
  %Move the k+1 | k+1 state estimate to the k|k estimate
  kf.ph_kgk = kf.ph_kp1gkp1;
  %Also for the state covariance estimate 
  kf.Ph_kgk = kf.Ph_kp1gkp1; 
end