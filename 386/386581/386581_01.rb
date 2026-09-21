#!/usr/bin/env ruby
# frozen_string_literal: true

# A386581: Number of normal multisets of size n with no permutation
#          having all distinct run lengths.
#
# 方針
#   大きさ n の正規多重集合は、重複度の列 (m_1, ..., m_k) すなわち n の組成と
#   一対一に対応する。条件は重複度の多重集合（分割 λ）だけで決まるので、
#
#     a(n) = Σ_{λ ⊢ n, λ が条件を満たさない} ℓ(λ)! / Π_j m_j(λ)!
#
#   として計算する。A386580 の補数列なので、n >= 1 では
#   a(n) = 2^(n-1) - A386580(n) も成り立つ（--check で照合する）。
#
# 分割 λ = (m_1, ..., m_k) が「ランの長さがすべて異なる並べ替え」をもつ ⇔
#   (1) 各 m_i を相異なる部分に分割 π_i し、全 π_i の部分が互いに重ならない
#   (2) r_i = ℓ(π_i) として 2 * max r_i <= Σ r_i + 1
#       （同じ値のランが隣接しないように並べられる条件）
#   を同時に満たす選び方がある。A386581 はこれが存在しないものを数える。
#
# 使い方
#   ruby 386581_01.rb [N]          # a(0..N) を表示（既定 N = 20）
#   ruby 386581_01.rb [N] --check  # 総当たり（n <= 8）と OEIS のデータ（n <= 15）で照合してから表示

# OEIS A386581 のデータ（n = 0..15）
OEIS_DATA = [0, 0, 1, 1, 5, 11, 20, 51, 108, 229, 448, 953, 1940, 3951, 7986, 15972].freeze

# n の分割を降順の配列で列挙する
def partitions(n, max = n, &block)
  return enum_for(:partitions, n, max) unless block
  if n.zero?
    yield []
    return
  end
  [n, max].min.downto(1) do |p|
    partitions(n - p, p) { |rest| yield [p] + rest }
  end
end

# m を相異なる部分に分割し、使用済み部分（ビットマスク used）と重ならないものについて
# [その分割の部分集合のマスク, 部分の個数] を yield する
def strict_splits(m, max, used, mask = 0, len = 0, &block)
  if m.zero?
    yield mask, len
    return
  end
  [m, max].min.downto(1) do |p|
    bit = 1 << p
    next if used & bit != 0
    strict_splits(m - p, p - 1, used | bit, mask | bit, len + 1, &block)
  end
end

# 分割 mults が条件 (1)(2) を満たす（= ランの長さがすべて異なる並べ替えをもつ）かどうか
def good?(mults, memo = {})
  key = mults.sort.reverse
  return memo[key] if memo.key?(key)

  # 重複度がすべて異なれば、各値を 1 つのランにまとめればよい
  return memo[key] = true if key.uniq.size == key.size

  found = false
  search = lambda do |i, used, r_sum, r_max|
    return if found
    if i == key.size
      found = true if 2 * r_max <= r_sum + 1
      return
    end
    strict_splits(key[i], key[i], used) do |mask, len|
      search.call(i + 1, used | mask, r_sum + len, [r_max, len].max)
      break if found
    end
  end
  search.call(0, 0, 0, 0)
  memo[key] = found
end

def factorial(k)
  (1..k).reduce(1, :*)
end

# 分割 λ の並べ替え（組成）の個数 ℓ(λ)! / Π m_j(λ)!
def compositions_count(lam)
  lam.tally.values.reduce(factorial(lam.size)) { |acc, c| acc / factorial(c) }
end

def a386581(n, memo = {})
  partitions(n).sum { |lam| good?(lam, memo) ? 0 : compositions_count(lam) }
end

# 補数列 A386580（照合用）
def a386580(n, memo = {})
  partitions(n).sum { |lam| good?(lam, memo) ? compositions_count(lam) : 0 }
end

# ---- 検証用：定義どおりの総当たり ----

# 多重集合の相異なる並べ替えを列挙する
def multiset_permutations(counts, acc = [], &block)
  if counts.all?(&:zero?)
    yield acc
    return
  end
  counts.each_index do |v|
    next if counts[v].zero?
    counts[v] -= 1
    acc.push(v)
    multiset_permutations(counts, acc, &block)
    acc.pop
    counts[v] += 1
  end
end

def distinct_run_lengths?(seq)
  lens = seq.chunk_while { |x, y| x == y }.map(&:size)
  lens.uniq.size == lens.size
end

# 正規多重集合（= 組成）を直接すべて調べ、条件を満たす並べ替えがないものを数える
def a386581_brute(n)
  return 0 if n.zero?
  count = 0
  (1 << (n - 1)).times do |bits|
    comp = []
    cur = 1
    (n - 1).times do |i|
      if bits[i] == 1
        comp << cur
        cur = 1
      else
        cur += 1
      end
    end
    comp << cur
    ok = false
    multiset_permutations(comp.dup) do |perm|
      if distinct_run_lengths?(perm)
        ok = true
        break
      end
    end
    count += 1 unless ok
  end
  count
end

# ---- メイン ----

if __FILE__ == $PROGRAM_NAME
  n_max = (ARGV.find { |s| s =~ /\A\d+\z/ } || 20).to_i
  memo = {}

  if ARGV.include?('--check')
    0.upto([n_max, 8].min) do |n|
      fast = a386581(n, memo)
      slow = a386581_brute(n)
      raise "brute-force mismatch at n=#{n}: #{fast} vs #{slow}" unless fast == slow
    end
    puts "brute-force check ok (n <= #{[n_max, 8].min})"

    OEIS_DATA.each_with_index do |v, n|
      got = a386581(n, memo)
      raise "OEIS mismatch at n=#{n}: #{got} vs #{v}" unless got == v
    end
    puts "OEIS data check ok (n <= #{OEIS_DATA.size - 1})"

    1.upto(n_max) do |n|
      raise "complement mismatch at n=#{n}" unless a386581(n, memo) + a386580(n, memo) == 1 << (n - 1)
    end
    puts "complement check ok: a(n) + A386580(n) = 2^(n-1) (1 <= n <= #{n_max})"
  end

  terms = (0..n_max).map { |n| a386581(n, memo) }
  terms.each_with_index { |v, n| puts "#{n} #{v}" }
  puts terms.join(', ')
end
