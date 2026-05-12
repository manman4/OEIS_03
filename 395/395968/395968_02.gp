

\\ Alkyl radicals B(x) = A000598
Balkyl(nmax)=
{
  my(B=1);
  for(i=1, nmax,
    B = 1 + x/6*(B^3 + 3*B*subst(B,x,x^2) + 2*subst(B,x,x^3)) + x*O(x^nmax)
  );
  B
};

\\ z_lambda for a partition lambda = [lambda_1, lambda_2, ...]
zpart(lambda)=
{
  my(v = Vec(lambda), z = 1, i = 1, j, m, part);
  while(i <= #v,
    part = v[i];
    m = 1;
    j = i + 1;
    while(j <= #v && v[j] == part, m++; j++);
    z *= part^m * m!;
    i = j;
  );
  z
};

\\ x * cycle_index(S_k, B(x)-1)
ASym(k, nmax)=
{
  my(B = Balkyl(nmax), s = 0, lambda, v, i, part, term);

  forpart(lambda = k,
    v = Vec(lambda);              \\ partition in nonincreasing order
    term = 1;
    for(i = 1, #v,
      part = v[i];
      term *= subst(B, x, x^part) - 1;
    );
    s += term / zpart(lambda);
  );

  x*s + x*O(x^nmax)
};

Vec(ASym(3, 30))
Vec(ASym(4, 30))
Vec(ASym(5, 30))

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
  for(m=1, nmax,
    \\ print(polcoef(B, m+1));
    P *= (1 - x^m + x*O(x^nmax))^(-polcoef(B, m+1))
  );
  P + x*O(x^nmax)
};

SlimLimit_prod(30)
Vec(SlimLimit_prod(30))