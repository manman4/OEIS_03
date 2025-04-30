# T(n,k) is the number of lattice paths from (0,0) to (k,n-k) using steps (4,0),(0,4),(3,3).
def A(n, k)
  # 配列を初期化 (k+1)*(n+1) サイズ
  dp = Array.new(k + 1){Array.new(n + 1, 0)}
  dp[0][0] = 1

  (0..k).each{|y|
    (0..n).each{|x|
      next if dp[y][x] == 0
      # (4,0) のステップ
      dp[y][x + 4] += dp[y][x] if x + 4 <= n
      # (0,4) のステップ
      dp[y + 4][x] += dp[y][x] if y + 4 <= k
      # (3,3) のステップ
      dp[y + 3][x + 3] += dp[y][x] if y + 3 <= k && x + 3 <= n
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