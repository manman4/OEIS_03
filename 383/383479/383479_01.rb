# T(n,k) is the number of lattice paths from (0,0) to (k,n-k) using steps (1,0),(3,0),(0,1).
def A(n, k)
  # 配列を初期化 (k+1)*(n+1) サイズ
  dp = Array.new(k + 1){Array.new(n + 1, 0)}
  dp[0][0] = 1

  (0..k).each{|y|
    (0..n).each{|x|
      next if dp[y][x] == 0
      # (1,0) のステップ
      dp[y][x + 1] += dp[y][x] if x + 1 <= n
      # (3,0) のステップ
      dp[y][x + 3] += dp[y][x] if x + 3 <= n
      # (0,1) のステップ
      dp[y + 1][x] += dp[y][x] if y + 1 <= k
    }
  }
  dp[k][n]
end

ary = []
(0..10).each{|i|
  (0..i).each{|j|
    ary << A(j, i-j)
  }
}
p ary

p (0..15).map{|i| A(i, i)}