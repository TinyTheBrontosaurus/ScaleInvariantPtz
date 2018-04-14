function kf=scaleMeasurementNoise(kf, scalar )
  kf.R_kp1 = kf.R * scalar;
end