#!/usr/bin/env ruby
# frozen_string_literal: true
#
# 349257_02.rb
#
# 問題:
#   [n] = {1, 2, ..., n} の順列 p に対して S(p) = Σ_{k=1}^{n} k / p(k)。
#   S(p) として表せる「最大の整数」を求める。
#
# 手法（厳密・分割統治 / meet-in-the-middle）:
#   * 全項を L = lcm(1..n) 倍して整数演算のみで扱う。d | L なので k*L/d は
#     整数。S が整数 ⇔ スケール和が L の倍数、値は 和/L。
#   * 位置(=分子)集合を 2 つの半分 A, B に分割する。
#       - B: すべての単射割り当てを列挙し、使った分母集合 mask ごとに
#            「剰余 (sumB mod L) → sumB の最大値」を表 res_b に圧縮。
#       - A: すべての単射割り当てを列挙して流し、相手側は分母集合の補集合。
#            sumA の剰余 r に対し need=(L-r) mod L の最大 sumB を引くと
#            sumA+sumB は必ず L の倍数（＝整数値）。その最大を取る。
#   * これで全 n! 通りを約 n!/⌈n/2⌉! + n!/⌊n/2⌋! 程度の列挙に圧縮する。
#     恒等順列が S=n を与えるので答えは常に n 以上（best の初期値）。
#
# 計算量: 時間・空間ともおおよそ O(n!/(n/2)!)。実用範囲はおおむね n <= 15
#   （n=13: 数秒, n=14: 数十秒, n=15: 数分。メモリは res_b が支配的）。
#
# 使い方:
#   ruby 349257_02.rb [n]      # 指定 n の答えを 1 個出力
#   ruby 349257_02.rb          # n = 1..13 の表を出力

def largest_integer_ratio_sum(n)
  raise ArgumentError, "n must be >= 1" if n < 1

  l = (1..n).reduce(1) { |acc, d| acc.lcm(d) } # L = lcm(1..n)
  term = Array.new(n + 1) { Array.new(n + 1, 0) }
  (1..n).each { |k| (1..n).each { |d| term[k][d] = k * l / d } } # 整数（d|L）

  b = n / 2
  a = n - b                       # A は大きい方（流す）、B は小さい方（表に圧縮）
  af = (1..a).to_a                # A に割り当てる位置
  bf = (a + 1..n).to_a            # B に割り当てる位置
  full = (1 << n) - 1             # 全分母のビット集合

  # --- B 側: 使用分母集合 mask ごとに {剰余 => sumB の最大} を作る ---
  res_b = {}
  build_b = lambda do |idx, mask, sum|
    if idx == bf.length
      h = (res_b[mask] ||= {})
      r = sum % l
      cur = h[r]
      h[r] = sum if cur.nil? || sum > cur
      return
    end
    k = bf[idx]
    d = 1
    while d <= n
      bit = 1 << (d - 1)
      build_b.call(idx + 1, mask | bit, sum + term[k][d]) if (mask & bit).zero?
      d += 1
    end
  end
  build_b.call(0, 0, 0)

  # --- A 側: 全割り当てを流し、補集合側 B の剰余表と突き合わせる ---
  best = n * l                    # 恒等順列 p(k)=k は S=n（下界）
  stream_a = lambda do |idx, mask, sum|
    if idx == af.length
      h = res_b[full ^ mask]      # A が使わなかった分母集合 = B の集合
      return if h.nil?
      need = (l - sum % l) % l     # sumA+sumB ≡ 0 (mod L) となる剰余
      v = h[need]
      best = sum + v if v && sum + v > best
      return
    end
    k = af[idx]
    d = 1
    while d <= n
      bit = 1 << (d - 1)
      stream_a.call(idx + 1, mask | bit, sum + term[k][d]) if (mask & bit).zero?
      d += 1
    end
  end
  stream_a.call(0, 0, 0)

  best / l
end

if ARGV.empty?
  puts "  n | largest integer"
  puts "----+----------------"
  (1..13).each { |n| printf("%3d | %d\n", n, largest_integer_ratio_sum(n)) }
else
  puts largest_integer_ratio_sum(Integer(ARGV[0]))
end
