M=40;

\\ Expansion of (1/x) * Series_Reversion( x * ((1-x^2)^2 - x) ).
my(N=M, x='x+O('x^N)); Vec( serreverse( x * ((1-x^2)^2 - x) )/x )