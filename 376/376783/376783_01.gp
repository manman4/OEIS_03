\\ 1/sqrt((1 - x^2 - x^5)^2 - 4*x^7).
my(N=80, x='x+O('x^N)); Vec( 1/sqrt((1-x^2-x^5)^2-4*x^7) )

\\ 1/sqrt((1 - x^2 + x^5)^2 - 4*x^5)
my(N=80, x='x+O('x^N)); Vec( 1/sqrt((1-x^2+x^5)^2-4*x^5) )

\\ 1/sqrt((1 + x^2 - x^5)^2 - 4*x^2)
my(N=80, x='x+O('x^N)); Vec( 1/sqrt((1+x^2-x^5)^2-4*x^2) )


