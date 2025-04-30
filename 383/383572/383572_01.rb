# T(n,k) is the number of lattice paths from (0,0) to (k,n-k) using steps (5,0),(0,5),(4,4).
def A(n, k)
  # 配列を初期化 (k+1)*(n+1) サイズ
  dp = Array.new(k + 1){Array.new(n + 1, 0)}
  dp[0][0] = 1

  (0..k).each{|y|
    (0..n).each{|x|
      next if dp[y][x] == 0
      # (5,0) のステップ
      dp[y][x + 5] += dp[y][x] if x + 5 <= n
      # (0,5) のステップ
      dp[y + 5][x] += dp[y][x] if y + 5 <= k
      # (4,4) のステップ
      dp[y + 4][x + 4] += dp[y][x] if y + 4 <= k && x + 4 <= n
    }
  }
  dp[k][n]
end

# ary = []
# (0..12).each{|i|
#   (0..i).each{|j|
#     ary << A(j, i-j)
#   }
# }
# p ary

p (0..35).map{|i| A(i, i)}