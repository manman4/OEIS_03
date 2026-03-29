def best_penalty(occ, size, facts)
  # occ 個を size 個の変数に分配するとき、
  # 係数を最大化する最小の分母 ∏(m_i!) を返す。
  # できるだけ均等に分けるのが最小になる。
  return 1 if occ == 0
  d = size < occ ? size : occ
  q, r = occ.divmod(d)
  # (q!)^(d-r) * ((q+1)!)^r
  facts[q] ** (d - r) * facts[q + 1] ** r
end

def precompute_tables(limit)
  max_total = limit + 1
  facts = [1]
  (1..max_total + 1).each { |i| facts << facts.last * i }

  # sizes[t] for t=0..limit
  sizes = Array.new(limit + 1, 0)
  sizes[0] = 1
  t = 1
  while t <= limit
    sizes[t] = 1 << (t - 1)
    t += 1
  end

  # penalty[t][occ] precompute up to max_total
  penalty = Array.new(limit + 1) { Array.new(max_total + 1, 1) }
  t = 0
  while t <= limit
    occ = 0
    while occ <= max_total
      penalty[t][occ] = best_penalty(occ, sizes[t], facts)
      occ += 1
    end
    t += 1
  end

  [penalty, facts]
end

def max_coeff_one(n, penalty)
  total = n + 1
  dp_num = Array.new(total + 1)
  dp_den = Array.new(total + 1)

  # t=0 init
  occ0 = 1
  while occ0 <= total
    dp_num[occ0] = occ0
    dp_den[occ0] = penalty[0][occ0]
    occ0 += 1
  end

  t = 1
  while t <= n
    new_num = Array.new(total + 1)
    new_den = Array.new(total + 1)
    # t-1 の時点で少なくとも t 個は解禁済みである必要がある
    r_prev = t
    while r_prev <= total
      prev_num = dp_num[r_prev]
      if prev_num
        prev_den = dp_den[r_prev]
        occ_min = t + 1 - r_prev
        occ_min = 0 if occ_min < 0
        occ_max = total - r_prev
        occ_t = occ_min
        while occ_t <= occ_max
          r = r_prev + occ_t
          factor = r - t
          val_num = prev_num * factor
          val_den = prev_den * penalty[t][occ_t]
          cur_num = new_num[r]
          if cur_num.nil?
            new_num[r] = val_num
            new_den[r] = val_den
          else
            cur_den = new_den[r]
            if val_num * cur_den > cur_num * val_den
              new_num[r] = val_num
              new_den[r] = val_den
            end
          end
          occ_t += 1
        end
      end
      r_prev += 1
    end
    dp_num = new_num
    dp_den = new_den
    t += 1
  end

  dp_num[total] / dp_den[total]
end

def solve_a393804(limit)
  penalty, _facts = precompute_tables(limit)
  n = 0
  while n <= limit
    a_n = max_coeff_one(n, penalty)
    break if a_n.to_s.size > 1000
    print n
    print ' '
    puts a_n
    n += 1
  end
end

# 証明コメント（Product_{j=0..n} (Sum_{k=1..2^j} x_k) の最大係数）:
# 1) 因子 j (0..n) は (x1+...+x_{2^j}) なので、
#    「時刻 j に 2^j 個の変数から 1 つ選ぶ」選択列と係数が一致する。
# 2) 変数 i が選べる最初の時刻は t(i)=ceil(log2(i)) (i=1 は t=0)。
# 3) 係数は「時刻制約つきの選択列の個数」なので、
#    (a) 時刻 t までに解禁された変数の個数を r とすると因子 (r - t) が掛かる
#    (b) 同一変数の重複は分母 ∏(m_i!) を生む
#    という形で表せる。
#    具体的には、時刻 0..n を順に処理するとき、時刻 t までに解禁された変数数が r なら、
#    次の因子の選択肢は「解禁済み r 個のうち、すでに選んだ t 個を除く」ので r - t 通り。
#    よって「時刻ごとの因子」は ∏_{t=0..n} (r_t - t) で表せる。
# 3') さらに指数ベクトルの係数は
#    coef = (選択列の総数) / ∏(m_i!)
#    となる。分母は同一変数が m_i 回出現したことによる並べ替えの重複数。
# 4) ある時刻 t に解禁される変数が size[t] 個あるとき、
#    その時刻で occ_t 回使うなら、∏(m_i!) を最小にするには
#    できるだけ均等に分配するのが最善（凸性の交換引数）。
# 5) よって「時刻ごとの使用回数 occ_t」を DP で全探索すれば、
#    最大係数が厳密に求められる（全て整数演算）。

solve_a393804(600)
