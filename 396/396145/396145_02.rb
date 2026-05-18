def count_partitions_with_rev_alt_sum(n)
  target_sum = 2 * n
  @memo = {}

  # 外側の変数に依存しないよう、ターゲットの -8 を直接比較するか、
  # 引数（またはインスタンス変数）として引き継ぐ必要があります。
  # ここでは直接 -8 と比較するように修正しました。
  
  def solve(last, remaining_sum, current_rev_alt)
    state = [last, remaining_sum, current_rev_alt]
    return @memo[state] if @memo.key?(state)

    if remaining_sum == 0
      # 1番小さいパーツ(奇数手目)は常にプラス、2番目はマイナス...
      # 最終的な反転交互和が -8 になっていれば 1 を返す
      return current_rev_alt == -8 ? 1 : 0
    end

    count = 0
    ((last + 1)..remaining_sum).each{|x|
      break if remaining_sum - x < 0
      
      # 次に選ぶパーツ（x）が何番目のパーツか（奇数番目か偶数番目か）を
      # 厳密に追跡するために、引数に len を追加します。
    }
  end

  # --- スコープ問題とロジックを完璧に整えた決定版 ---
  @memo = {}
  
  # 引数に len (現在のパーツ数) を持たせることで、
  # 奇数番目ならプラス、偶数番目ならマイナスという判定を確実に行います。
  def solve_final(last, remaining_sum, current_rev_alt, len)
    state = [last, remaining_sum, current_rev_alt, len]
    return @memo[state] if @memo.key?(state)

    if remaining_sum == 0
      return current_rev_alt == -8 ? 1 : 0
    end

    count = 0
    ((last + 1)..remaining_sum).each{|x|
      break if remaining_sum - x < 0
      
      next_len = len + 1
      # 1つ目のパーツ(next_len=1) -> +x
      # 2つ目のパーツ(next_len=2) -> -x
      next_rev_alt = current_rev_alt + (next_len.odd? ? x : -x)
      
      count += solve_final(x, remaining_sum - x, next_rev_alt, next_len)
    }

    @memo[state] = count
  end

  solve_final(0, target_sum, 0, 0)
end

# 実行
n = 60
p (0..n).map{|i| count_partitions_with_rev_alt_sum(i)}
