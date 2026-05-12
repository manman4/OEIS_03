\\ Alkyl radicals B(x) = A000598
Balkyl(n) = my(B=1); for(i=1, n, B=1+x/6*(B^3+3*B*subst(B, x, x^2)+2*subst(B, x, x^3))+x*O(x^n)); B;

SlimLimit(nmax)=
{
  my(B=Balkyl(nmax), S=0);
  for(j=1, nmax,
    S += ((subst(B,x,x^j) - 1)/x^j - 1)/j
  );
  x*exp(S) + x*O(x^nmax)
};

L = SlimLimit(30)

SlimLimit_prod(nmax)=
{
  my(B=Balkyl(nmax+1), P=1 + x*O(x^nmax));
  print(B);
  for(m=1, nmax,
    \\ print(polcoef(B, m+1));
    P *= (1 - x^m + x*O(x^nmax))^(-polcoef(B, m+1))
  );
  P + x*O(x^nmax)
};

SlimLimit_prod(30)
Vec(SlimLimit_prod(30))


print("For OEIS");

\\ G.f.: Product_{k>=1} 1/(1 - x^k)^A000598(k+1).
alkyl(n) = my(B=1); for(i=1, n, B=1+x/6*(B^3+3*B*subst(B, x, x^2)+2*subst(B, x, x^3))+x*O(x^n)); B;
alkyl(40)
Vec(alkyl(40))
my(N=40, x='x+O('x^N), B=alkyl(N+1)); Vec(1/prod(k=1, N, (1-x^k)^polcoef(B, k+1)))