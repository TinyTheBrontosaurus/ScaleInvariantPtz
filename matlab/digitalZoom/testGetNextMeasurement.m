function testGetNextMeasurement
[run, meas] = getNextMeasurement;
while( run )
    [run, data(900-run)]=getNextMeasurement(run,meas);
    meas = data(900-run);
end
plot(data)