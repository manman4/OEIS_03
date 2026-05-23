default(parisize, 120000000);

\\ T(n,k) = [x^k] (F_n(x)/x)^(1/3),
\\ where F_0(x) = x and F_{n+1}(x) = F_n(x) * (1 + F_n(x))^3.
\\ Using
\\   (F_n(x)/x)^(1/3) = prod(j = 0, n - 1, 1 + F_j(x)).

Fn(n) =
{
  my(x = 'x, m = (4^n - 1) / 3, F = x + x * O(x^m));
  for(i = 1, n, F = F * (1 + F)^3);
  F;
}

Trow(n) =
{
  my(x = 'x, m = (4^n - 1) / 3, P = 1 + x * O(x^m), F = x + x * O(x^m));
  for(j = 0, n - 1,
    P = P * (1 + F);
    F = F * (1 + F)^3;
  );
  Vec(P);
}

T(n, k) =
{
  if(k < 0 || k > (4^n - 1) / 3, error("0 <= k <= (4^n-1)/3 must hold"));
  Trow(n)[k + 1];
}

triangle(N) =
{
  my(v = []);
  for(n = 0, N, v = concat(v, Trow(n)));
  v;
}

write_triangle_data(N, filename) =
{
  my(v = triangle(N));
  system(Str("rm -f ", filename));
  for(i = 1, #v,
    write(filename, Str(i - 1, " ", v[i]))
  );
}

\\ example
N = 7;
write_triangle_data(N, "b396277_1.txt");

