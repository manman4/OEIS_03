my(N=20, x='x+O('x^N)); concat(0, Vec(serlaplace(sum(k=1, N, (1 - exp(-k*x))^k / k^3 ))))



a(n, m) = sum(k=1, n, (-1)^(n-k)*k!*k^(n-m)*stirling(n, k, 2));

my(N=20, x='x+O('x^N)); serlaplace( sum(k=0, N, a(k+1, 2)*x^k/k! - 1   * (1 - exp(-k*x))^k ))

\\ Sum_{k>=0} a(k+2) * x^k/k!　= Sum_{k>=0} k * (1 - exp(-k*x))^k.
my(N=20, x='x+O('x^N)); serlaplace( sum(k=0, N, a(k+2, 3)*x^k/k! - k   * (1 - exp(-k*x))^k ))

my(N=20, x='x+O('x^N)); serlaplace( sum(k=0, N, a(k+3, 4)*x^k/k! - k^2 * (1 - exp(-k*x))^k ))

my(N=20, x='x+O('x^N)); serlaplace( sum(k=0, N, a(k+4, 5)*x^k/k! - k^3 * (1 - exp(-k*x))^k ))

my(N=20, x='x+O('x^N)); serlaplace( sum(k=0, N, a(k+5, 6)*x^k/k! - k^4 * (1 - exp(-k*x))^k ))
