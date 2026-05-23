\\ T(n,k) = [x^k] (F_n(x)/x)^(1/2),
\\ where F_0(x) = x and F_{n+1}(x) = F_n(x) * (1 + F_n(x))^2.
\\ If P_n(x) = (F_n(x)/x)^(1/2), then
\\ P_0(x) = 1, P_{n+1}(x) = P_n(x) * (1 + x * P_n(x)^2).

Trow(n) =
{
  my(x = 'x, m = (3^n - 1) / 2, P = 1 + x * O(x^m));
  for(i = 1, n, P = P * (1 + x * P^2));
  Vec(P);
}

T(n, k) =
{
  if(k < 0 || k > (3^n - 1) / 2, error("0 <= k <= (3^n-1)/2 must hold"));
  Trow(n)[k + 1];
}

\\ flatten rows n = 0..N
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
N = 8;
write_triangle_data(N, "b396276.txt");

