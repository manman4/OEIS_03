M=40;

\\ Expansion of 1 / ((1-x)^9 - 9*x)^(1/3).
my(N=M, x='x+O('x^N)); Vec( 1 / ((1-x)^9 - 9*x)^(1/3) )
