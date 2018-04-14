function kf=measurement(kf)
  kf.nu_kp1 = kf.m_kp1 - kf.mh_kp1gk;
end


