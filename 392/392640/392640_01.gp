M=40;

\\ Expansion of 1 / sqrt((1-x)^4 - 4*x^3).
my(N=M, x='x+O('x^N)); Vec( 1 / sqrt((1-x)^4 - 4*x^3) )
