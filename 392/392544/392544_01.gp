M=40;

\\ Expansion of 1 / ((1-x)^3 - x^7).
my(N=M, x='x+O('x^N)); Vec( 1 / ((1-x)^3 - x^7) )
