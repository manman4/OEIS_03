\\ T(n,k) = Sum_{0 = x_0 <= x_1 <= ... <= x_{n-1} <= x_n = k}
\\          Product_{j=0..n-1} binomial(2*x_j + 1, x_{j+1} - x_j).
\\
\\ This gives the recurrence
\\   T(0,0) = 1,
\\   T(n,k) = sum(m = 0, k, T(n-1,m) * binomial(2*m + 1, k - m)).

Trow(n) =
{
  if(n == 0, return([1]));
  my(prev = [1], curr, m);
  for(step = 1, n,
    m = (3^step - 1) / 2;
    curr = vector(m + 1, k,
      my(s = 0);
      for(j = 0, k - 1,
        if(j <= #prev - 1,
          s += prev[j + 1] * binomial(2*j + 1, (k - 1) - j)
        )
      );
      s
    );
    prev = curr;
  );
  prev;
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
write_triangle_data(N, "b396276_2.txt");
