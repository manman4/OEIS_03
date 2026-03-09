class A320348Fast
  def initialize
    @memo = {}
    @result_memo = { 0 => 0 }
    @tetra = [0]
    @div_upper = {}
  end

  # target: 残りの合計値
  # current_weight: 現在考えている重み (m, m-1, ..., 1)
  # used_values_mask: すでに d_i として使用した数値のビットマスク
  def solve(target, current_weight, used_values_mask)
    # 終了条件: 重みが0になったとき、ちょうどtargetが0なら1通り
    return target == 0 ? 1 : 0 if current_weight == 0

    # 下界枝刈り: 使える最小の未使用値から作る最小和
    lower = min_possible_sum(current_weight, used_values_mask)
    return 0 if target < lower

    # 上界枝刈り: target 以下の未使用値から作る最大和
    upper = max_possible_sum(current_weight, target, used_values_mask)
    return 0 if target > upper

    by_mask = (@memo[used_values_mask] ||= {})
    by_weight = (by_mask[current_weight] ||= {})
    cached = by_weight[target]
    return cached unless cached.nil?

    count = 0

    # 次の「差」d_i を選ぶ
    # d_i * current_weight が target を超えない範囲でループ
    d_upper = upper_d(target, current_weight)
    d = 1
    while d <= d_upper
      bit = (1 << d)
      if (used_values_mask & bit) == 0
        rest = target - (d * current_weight)
        # 次段の最小可能和で追加枝刈り
        if rest >= min_possible_sum(current_weight - 1, used_values_mask | bit)
          count += solve(rest, current_weight - 1, used_values_mask | bit)
        end
      end
      d += 1
    end

    by_weight[target] = count
  end

  def calculate(n)
    return @result_memo[n] if @result_memo.key?(n)

    total_count = 0
    # パーツの数 m を 1 から順に試す
    # m 個の最小の差 (1,2,..,m) での最小合計は m*(m+1)*(m+2)/6 (四面体数)
    # これが n を超えるまでループ
    m = 1
    loop do
      min_sum = m * (m + 1) * (m + 2) / 6
      break if min_sum > n

      total_count += solve(n, m, 0)
      m += 1
    end
    @result_memo[n] = total_count
    total_count
  end

  def each_value(max_n)
    return enum_for(:each_value, max_n) unless block_given?
    i = 0
    while i <= max_n
      yield i, calculate(i)
      i += 1
    end
  end

  private

  def tetra(k)
    while @tetra.length <= k
      i = @tetra.length
      @tetra << (i * (i + 1) * (i + 2) / 6)
    end
    @tetra[k]
  end

  # 未使用の小さい値から w 個選んだときの最小和
  def min_possible_sum(w, used_values_mask)
    return 0 if w <= 0

    s = 0
    d = 1
    weight = w
    while weight >= 1
      bit = (1 << d)
      if (used_values_mask & bit) == 0
        s += weight * d
        weight -= 1
      end
      d += 1
    end
    s
  end

  # d <= d_cap の未使用値から w 個選んだときの最大和
  def max_possible_sum(w, d_cap, used_values_mask)
    return 0 if w <= 0
    return -1 if d_cap <= 0

    s = 0
    d = d_cap
    weight = w
    while d >= 1 && weight >= 1
      bit = (1 << d)
      if (used_values_mask & bit) == 0
        s += weight * d
        weight -= 1
      end
      d -= 1
    end
    return -1 if weight >= 1
    s
  end

  # 3) target / weight の上限を weight ごとに事前計算して再利用
  def upper_d(target, weight)
    cache = (@div_upper[weight] ||= [0])
    t = cache.length
    while t <= target
      cache << (t / weight)
      t += 1
    end
    cache[target]
  end
end

# 実行
solver = A320348Fast.new

# 実行
n = 200
solver.each_value(n) do |i, v|
  next if i == 0
  break if i > n
  print i
  print " "
  puts v
end
