\\ T(n,k) = [x^k] (F_n(x)/x)^(1/2),
\\ where F_0(x) = x and F_{n+1}(x) = F_n(x) * (1 + F_n(x))^2.
\\ Using
\\   (F_n(x)/x)^(1/2) = prod(j = 0, n - 1, 1 + F_j(x)).

\\ Return F_n(x) truncated at degree (3^n - 1)/2.
Fn(n) =
{
  my(x = 'x, m = (3^n - 1) / 2, F = x + x * O(x^m));
  for(i = 1, n, F = F * (1 + F)^2);
  F;
}

Trow(n) =
{
  my(x = 'x, m = (3^n - 1) / 2, P = 1 + x * O(x^m), F = x + x * O(x^m));
  for(j = 0, n - 1,
    P = P * (1 + F);
    F = F * (1 + F)^2;
  );
  Vec(P);
}

T(n, k) =
{
  if(k < 0 || k > (3^n - 1) / 2, error("0 <= k <= (3^n-1)/2 must hold"));
  Trow(n)[k + 1];
}

triangle(N) =
{
  my(v = []);
  for(n = 0, N, v = concat(v, Trow(n)));
  v;
}

\\ example
print(triangle(4));
