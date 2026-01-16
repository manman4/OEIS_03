M=40;

\\ Expansion of 1 / ((1-x)^2 - x^6).
my(N=M, x='x+O('x^N)); Vec( 1 / ((1-x)^2 - x^6) )
