%  Reference solution to convert from voltage to temperature in degrees Celsisus

function [tempC, Debug] = volts2degc( sensorVoltage )

%  Function volts2degc
%  This function converts the LM61 output voltage to a temperature value in degrees C
%  The conversion is described by the equation T = (Vinput - 0.6V)/ (10 mV/degC )
%
%  Input Arguments
%  sensorVoltage -- Output voltage from the sensor in volts
%
%  Output Arguments
%  tempC -- The temperature in degrees C measured by the sensor
%
%  Optional Output Arguments
%  Debug -- A MATLAB structure that is used to pass internal function values out to the
%           calling routine.  This can be used to check intermediate values.  It is not
%           required to be used and should not be used in internal calculations
%

%  Declare the Debug structure
Debug = struct;


 %  Define two constants, one for the offset voltage and one for the sensor slope

offsetVoltage = 0.6 %  Offset voltage in volts
sensorSlope = 10 * 10^-3 %  Sensor slope in Volts per degC (-10 mV/degC)

%  Use the Debug structure to pass back the constants off offset voltage and the sensor slope to verify
%  verify they are correct.
%  These two lines are locked because the assessment uses them

Debug.offsetVoltage = offsetVoltage;
Debug.sensorSlope = sensorSlope;


%  Compute the temperature from the sensorVoltage using the given relationship and constants.
%  The result is returned as tempC

tempC = (sensorVoltage - offsetVoltage) / sensorSlope;

end