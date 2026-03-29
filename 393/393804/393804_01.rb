def max_coefficient(n)
  # dp[s] は、これまでに s 個の変数を選んだときの「係数の最大値」
  dp = { 0 => 1 }

  # 階乗計算のヘルパー
  fact = ->(x) { (2..x).inject(1, :*) }

  (0..n).each{|k|
    next_dp = Hash.new(0)
    
    # レベル k で利用可能な変数の種類数 (vk)
    vk = (k == 0) ? 1 : 2**(k - 1)

    dp.each{|s, val|
      # 理論上、レベル k に到達した時点で s は k 以上でなければならない
      next if s < k

      # 今回新しく選ぶ変数の数 c を探索
      max_c = n + 1 - s
      (0..max_c).each{|c|
        new_s = s + c
        
        # 変数を追加したあとの総数 new_s は k + 1 以上でなければならない
        next if new_s < k + 1

        # 1. 配置の自由度による順列の増加分: (s + c - k)! / (s - k)!
        num = 1
        (1..c).each { |i| num *= (s - k + i) }

        # 2. 変数の重複による除数（分母）: 均等に分配して階乗の積を最小化
        q = c / vk
        r = c % vk
        dk = (fact.call(q + 1)**r) * (fact.call(q)**(vk - r))

        # 3. マルチプライアを掛けて最大値を更新（割り切れることが数学的に保証されています）
        multiplier = num / dk
        new_val = val * multiplier
        
        if new_val > next_dp[new_s]
          next_dp[new_s] = new_val
        end
      }
    }
    dp = next_dp
  }

  # 全グループ (n + 1 個) 分の変数を選び終えたときが答え
  dp[n + 1]
end

# テスト実行
# (0..20).each{|n|
#   puts "n = #{n.to_s.rjust(2)} : 最大係数 = #{max_coefficient(n)}"
# }
p (0..30).map{|n| max_coefficient(n)}