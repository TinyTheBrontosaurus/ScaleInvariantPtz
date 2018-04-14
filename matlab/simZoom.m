s = tf('s');
delayImageProc = .100;
framesPerZoom = 1;
periodFrame = 0.033;

pZoom = 1/(1+s/43.3)^4;
delayInquiryZoom = delayImageProc;
periodZoomControl = periodFrame * framesPerZoom;
rateLimitZoom = 14130;

