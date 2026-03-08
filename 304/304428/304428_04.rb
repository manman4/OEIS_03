def solve_a304428(max_n)
  # メモ化用のハッシュ
  @memo = {}

  # n: 残りの合計
  # prev_sum: 直前のグループの和 (s_i-1)
  # min_part: 次に使える最小のパーツの値 (d_i > d_i-1 を保証)
  def count_partitions(n, prev_sum, min_part)
    return 1 if n == 0
    
    state = [n, prev_sum, min_part]
    return @memo[state] if @memo.key?(state)

    count = 0

    # 次のパーツ d を選ぶ (前のパーツより大きくする必要がある)
    (min_part..n).each{|d|
      # パーツ d を k 個使う
      (1..(n / d)).each{|k|
        current_sum = d * k
        
        # 条件：現在のグループの和が直前の和より大きいこと
        if current_sum > prev_sum
          count += count_partitions(n - current_sum, current_sum, d + 1)
        end
      }
    }

    @memo[state] = count
  end

  (0..max_n).map do |i|
    @memo = {} # n ごとにリセット、もしくは累積して使う
    count_partitions(i, 0, 1)
  end
end

n = 100
results = solve_a304428(n)
(0..n).each{|i|
  print i
  print ' '
  puts results[i]
}