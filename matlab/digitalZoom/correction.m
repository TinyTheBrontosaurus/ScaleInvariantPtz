function kf=correction(kf)
  kf.W_kp1 = kf.Ph_kp1gk * kf.H_kp1' * inv(kf.Mh_kp1);
  kf.ph_kp1gkp1 = kf.ph_kp1gk + kf.W_kp1 * kf.nu_kp1;
  kf.Ph_kp1gkp1 = kf.Ph_kp1gk - ...
                      kf.W_kp1 * kf.Mh_kp1 * kf.W_kp1';
end

