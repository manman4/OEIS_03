class A320348Fast
  def initialize
    @memo = {}
  end

  # target: 残りの合計値
  # current_weight: 現在考えている重み (m, m-1, ..., 1)
  # used_values_mask: すでに d_i として使用した数値のビットマスク
  def solve(target, current_weight, used_values_mask)
    # 終了条件: 重みが0になったとき、ちょうどtargetが0なら1通り
    return target == 0 ? 1 : 0 if current_weight == 0
    
    state = [target, current_weight, used_values_mask]
    return @memo[state] if @memo.key?(state)

    count = 0
    
    # 次の「差」d_i を選ぶ
    # d_i * current_weight が target を超えない範囲でループ
    1.upto(target / current_weight){|d|
      # d がまだ使われていなければ採用
      if (used_values_mask & (1 << d)) == 0
        count += solve(target - (d * current_weight), current_weight - 1, used_values_mask | (1 << d))
      end
    }

    @memo[state] = count
  end

  def calculate(n)
    total_count = 0
    # パーツの数 m を 1 から順に試す
    # m 個の最小の差 (1,2,..,m) での最小合計は m*(m+1)*(m+2)/6 (四面体数)
    # これが n を超えるまでループ
    m = 1
    loop do
      min_sum = m * (m + 1) * (m + 2) / 6
      break if min_sum > n
      
      @memo = {}
      total_count += solve(n, m, 0)
      m += 1
    end
    total_count
  end
end

# 実行
solver = A320348Fast.new

# 実行
n = 100
results = (0..n).map {|i| solver.calculate(i)}

(1..n).each{|i|
  print i
  print " "
  puts results[i]
}
