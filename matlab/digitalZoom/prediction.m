function kf=prediction(kf)

  %Update state prediction
  kf.ph_kp1gk = kf.F_k * kf.ph_kgk + kf.u_k;
  
  %Update state prediction covariance
  kf.Ph_kp1gk = kf.F_k * kf.Ph_kgk * kf.F_k' + kf.Q_k;
  
  %Update measurement prediction
  kf.mh_kp1gk = kf.H_kp1 * kf.ph_kp1gk;
  
  %Update measurement prediction covariance
  kf.Mh_kp1 = kf.H_kp1 * kf.Ph_kp1gk * kf.H_kp1' + kf.R_kp1;

end

