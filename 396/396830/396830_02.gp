\\ Multiplicative with a(p^e) = ((p^4-1)*(p^(2*e)*(p^(3*e)-1)/(p^3-1) + p*(p^(5*e)-1)/(p^5-1)) + p^(2*e+1) + 1)/(p+1).
a_pp(p, e) =
{
  if(e == 0, return(1));
  ((p^4 - 1) * (p^(2 * e) * (p^(3 * e) - 1) / (p^3 - 1) + p * (p^(5 * e) - 1) / (p^5 - 1)) + p^(2 * e + 1) + 1)
    / (p + 1)
};


a(n) =
{
  if(n <= 0, return(0));
  my(f = factor(n), rows = matsize(f)[1]);
  prod(i = 1, rows, a_pp(f[i, 1], f[i, 2]))
};

for(n=1, 50, print1(a(n), ", "));