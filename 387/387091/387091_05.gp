\\ G.f.: B(x)^2/(1 + 8*(B(x)-1)/9), where B(x) is the g.f. of A169958.
my(N=30, x='x+O('x^N), b=sum(k=0, N, binomial(9*k, k)*x^k)); Vec( b )
my(N=30, x='x+O('x^N), b=sum(k=0, N, binomial(9*k, k)*x^k)); Vec( b^2/(1 + 8*(b-1)/9) )