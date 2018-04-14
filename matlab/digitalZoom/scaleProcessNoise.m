function kf=scaleProcessNoise(kf, scalar )
  kf.Q_k = kf.Q * scalar;
end