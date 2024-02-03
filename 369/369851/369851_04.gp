\\ offset = 1
my(N=40, x='x+O('x^N)); concat([0, 0], Vec( x^3*(1-x)/((1-x)^5 - x^4) ))
