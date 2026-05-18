def count_partitions_with_rev_alt_sum_minus_6(n)
  target_sum = 2 * n
  @memo = {}
  
  # last: 直前に選んだパーツ
  # remaining_sum: 残りの必要な総和
  # current_rev_alt: これまでの反転交互和の計算値
  # len: これまでに選んだパーツの数
  def solve(last, remaining_sum, current_rev_alt, len)
    state = [last, remaining_sum, current_rev_alt, len]
    return @memo[state] if @memo.key?(state)

    if remaining_sum == 0
      # 最終的な反転交互和が -6 になっているものをカウント
      return current_rev_alt == -6 ? 1 : 0
    end

    count = 0
    ((last + 1)..remaining_sum).each{|x|
      break if remaining_sum - x < 0
      
      next_len = len + 1
      # 小さい順に選ぶため、1つ目(最小)はプラス、2つ目はマイナス、3つ目はプラス... となる
      next_rev_alt = current_rev_alt + (next_len.odd? ? x : -x)
      
      count += solve(x, remaining_sum - x, next_rev_alt, next_len)
    }

    @memo[state] = count
  end

  solve(0, target_sum, 0, 0)
end

p (0..60).map{|i| count_partitions_with_rev_alt_sum_minus_6(i)}
