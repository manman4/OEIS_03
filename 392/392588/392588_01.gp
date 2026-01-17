M=40;

\\ Expansion of 1 / ((1-x)^4 - x^4)^2.
my(N=M, x='x+O('x^N)); Vec( 1 / ((1-x)^4 - x^4)^2 )
