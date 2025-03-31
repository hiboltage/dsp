function [yOut, Debug] = Convolution( inputX, impulseH )
%
%  The myConvolution function convolves the input sequence 'inputX' and the
%  system impulse response 'impulseH' to result in the output value yOut.
%
%  The output of the convolution is the sequence yOut.  If the inputX has a
%  length of M and the impulseH has a length of N, then the output sequency
%  yOut has a length of M+N-1
%
%
%  Input Arguments
%
%  inputX -- input sequence of length M
%  impulseH -- impulse response of length N
%
%  yOut -- result of the convolution of length M+N-1
%
%  Optional Arguments
%
%  Debug -- a structure that can be used to pass intermediate values back
%  to the calling routine for debug purposes.  Use this to help debug your
%  function.  Set internal variables using fields of the structure.  These
%  are created using the MATLAB 'dot' notation.  For example if I want to
%  check a a val
%

%  Define the Debug structure variable (lock this line)
Debug = struct;

%  Find the length of the input sequence and the impulse response

M = length(inputX);    % Length of the first input argument inputX
N = length(impulseH);  % Length of the second input argument impulseH



%  Compute the length of the output sequence is M+N-1
convLength = M + N - 1;

%  Initialize the output sequence to all zeros
yOut = zeros(1,convLength);

%  Implement the convolution routine.  It may be helpful to
%  follow the BASIC function from your text
%
%  This routine follows the BASIC routine from:
%  "The Scientist and Engineer's Guide to Digital Signal Processing"
%  Steven W. Smith
%  pp. 121

%  Place your code to perform convolution using the output side algorithm
%  here
    for i = 1:convLength  % iterate over the output signal
        yOut(i) = 0;  % zero the sample in the output array (unnecessary because of line 38)

        % -------------- Kernel -------------- %
        for j = 1:N  % iterate over the impulse response (kernel)

            if (i-j + 1) < 1  % make sure an array index of less than 1 is not passed to inputX
                continue
            elseif (i-j + 1) > M  % if difference between iterators is greater than length of the input sequence minus one
                continue
            else
                yOut(i) = yOut(i) + impulseH(j) * inputX(i-j + 1);  % multiply input by impulse response and sum each multiple moving from right to left
            end
           
        end
        % ------------------------------------ %
        
    end

end