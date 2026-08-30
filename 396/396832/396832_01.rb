# Number of permutations sigma of [n] with k * sigma(k) >= n-1 for 1 <= k <= n.
# 積公式は使わず、定義から直接数える。

# --- 1) 定義そのまま: 全順列を列挙して条件を判定 (n <= 9 程度) ---
def a_brute(n)
  return 1 if n.zero?
  (1..n).to_a.permutation.count do |s|
    (1..n).all? { |k| k * s[k - 1] >= n - 1 }
  end
end

# --- 2) バックトラック: 位置 1..n を順に埋め、途中で条件を判定 (n <= 12 程度) ---
def a_backtrack(n)
  return 1 if n.zero?
  used = Array.new(n + 1, false)

  rec = lambda do |k|
    return 1 if k > n
    total = 0
    (1..n).each do |v|
      next if used[v]
      next if k * v < n - 1
      used[v] = true
      total += rec.call(k + 1)
      used[v] = false
    end
    total
  end

  rec.call(1)
end

# --- 3) ビットマスク DP: 使用済み値の集合で状態をまとめる (n <= 20 程度) ---
def a_dp(n)
  return 1 if n.zero?
  dp = Array.new(1 << n, 0)
  dp[0] = 1
  (0...(1 << n)).each do |mask|
    ways = dp[mask]
    next if ways.zero?
    k = mask.to_s(2).count("1") + 1   # 次に埋める位置
    (1..n).each do |v|
      bit = 1 << (v - 1)
      next if mask & bit != 0
      next if k * v < n - 1
      dp[mask | bit] += ways
    end
  end
  dp[(1 << n) - 1]
end

if __FILE__ == $PROGRAM_NAME
  puts "n : brute  backtrack  dp"
  (0..9).each do |n|
    printf("%2d: %6d %10d %6d\n", n, a_brute(n), a_backtrack(n), a_dp(n))
  end
  puts
  puts "n = 0..20 (dp):"
  p (0..20).map { |n| a_dp(n) }
end
