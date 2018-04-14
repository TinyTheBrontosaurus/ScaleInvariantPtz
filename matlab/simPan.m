s = tf('s');
delayImageProc = .095;
framesPerPan = 2;
periodFrame = 0.033;

pPan = 1/(1+s/15)^2;
delayInquiryPan = .015;
periodPanControl = periodFrame * framesPerPan;
rateLimitPan = 290;
gainPixelsPerDegree = 4;
