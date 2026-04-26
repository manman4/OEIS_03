# T(n,k): derangement のうち ascents が k 個の個数
# 漸化式: T(n,k) = (k+1)*T(n-1,k) + (n-k)*T(n-1,k-1) + (-1)^n*(k==0)
# a(n) = T(n,2) を O(n) で計算

n_max = 30

# dp[j] = T(current_n, j) for j = 0,1,2
t0, t1, t2 = 1, 0, 0  # T(0, 0..2)

result = [t2]

(1..n_max).each do |n|
  sign = n.even? ? 1 : -1
  nt0 = t0 + sign              # (0+1)*T(n-1,0) + (-1)^n
  nt1 = 2 * t1 + (n - 1) * t0 # (1+1)*T(n-1,1) + (n-1)*T(n-1,0)
  nt2 = 3 * t2 + (n - 2) * t1 # (2+1)*T(n-1,2) + (n-2)*T(n-1,1)
  t0, t1, t2 = nt0, nt1, nt2
  result << t2
end

p result
