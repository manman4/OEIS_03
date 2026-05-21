default(parisize, 120000000);

\\ T(n,k) = [x^k] (F_n(x)/x)^(1/3),
\\ where F_0(x) = x and F_{n+1}(x) = F_n(x) * (1 + F_n(x))^3.
\\ If P_n(x) = (F_n(x)/x)^(1/3), then
\\ P_0(x) = 1, P_{n+1}(x) = P_n(x) * (1 + x * P_n(x)^3).

Trow(n) =
{
  my(x = 'x, m = (4^n - 1) / 3, P = 1 + x * O(x^m));
  for(i = 1, n, P = P * (1 + x * P^3));
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
write_triangle_data(N, "b396277.txt");
