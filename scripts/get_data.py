#!/usr/bin/env python3
"""
Script to fetch historical stock data from Yahoo Finance API
and save it to CSV format for the C++ trading optimizer.

Usage:
    python3 get_data.py ENEL.MI 2y
    python3 get_data.py UCG.MI 5y
"""

import sys
sys.path.append('/opt/.manus/.sandbox-runtime')
from data_api import ApiClient
import csv
import os
from datetime import datetime

def fetch_stock_data(symbol, time_range='2y', output_dir='../data'):
    """
    Fetch stock data from Yahoo Finance API and save to CSV.
    
    Args:
        symbol: Stock symbol (e.g., 'ENEL.MI' for Enel on Milan exchange)
        time_range: Time range (1d, 5d, 1mo, 3mo, 6mo, 1y, 2y, 5y, 10y, ytd, max)
        output_dir: Directory to save the CSV file
    """
    print(f"Fetching data for {symbol} with range {time_range}...")
    
    client = ApiClient()
    
    try:
        response = client.call_api('YahooFinance/get_stock_chart', query={
            'symbol': symbol,
            'region': 'IT',  # Italy
            'interval': '1d',  # Daily data
            'range': time_range,
            'includeAdjustedClose': True,
            'events': 'div,split'
        })
        
        if not response or 'chart' not in response or 'result' not in response['chart']:
            print(f"Error: No data received for {symbol}")
            return False
        
        result = response['chart']['result'][0]
        meta = result['meta']
        timestamps = result['timestamp']
        quotes = result['indicators']['quote'][0]
        
        print(f"Company: {meta.get('longName', 'N/A')}")
        print(f"Exchange: {meta['exchangeName']}")
        print(f"Currency: {meta['currency']}")
        print(f"Data points: {len(timestamps)}")
        
        # Prepare output directory
        script_dir = os.path.dirname(os.path.abspath(__file__))
        output_path = os.path.join(script_dir, output_dir)
        os.makedirs(output_path, exist_ok=True)
        
        # Save to CSV
        csv_filename = os.path.join(output_path, f"{symbol}.csv")
        
        with open(csv_filename, 'w', newline='') as csvfile:
            writer = csv.writer(csvfile)
            writer.writerow(['timestamp', 'open', 'high', 'low', 'close', 'volume'])
            
            for i in range(len(timestamps)):
                timestamp = timestamps[i]
                open_price = quotes['open'][i] if quotes['open'][i] else 0
                high_price = quotes['high'][i] if quotes['high'][i] else 0
                low_price = quotes['low'][i] if quotes['low'][i] else 0
                close_price = quotes['close'][i] if quotes['close'][i] else 0
                volume = quotes['volume'][i] if quotes['volume'][i] else 0
                
                writer.writerow([timestamp, open_price, high_price, low_price, close_price, volume])
        
        print(f"Data saved to: {csv_filename}")
        return True
        
    except Exception as e:
        print(f"Error fetching data: {str(e)}")
        return False

def main():
    if len(sys.argv) < 2:
        print("Usage: python3 get_data.py <SYMBOL> [TIME_RANGE]")
        print("\nExamples:")
        print("  python3 get_data.py ENEL.MI 2y")
        print("  python3 get_data.py UCG.MI 5y")
        print("\nPopular Italian stocks:")
        print("  ENEL.MI  - Enel SpA")
        print("  ENI.MI   - Eni SpA")
        print("  ISP.MI   - Intesa Sanpaolo")
        print("  UCG.MI   - UniCredit")
        print("  G.MI     - Generali")
        print("  TIT.MI   - Telecom Italia")
        print("\nTime ranges: 1d, 5d, 1mo, 3mo, 6mo, 1y, 2y, 5y, 10y, ytd, max")
        sys.exit(1)
    
    symbol = sys.argv[1]
    time_range = sys.argv[2] if len(sys.argv) > 2 else '2y'
    
    success = fetch_stock_data(symbol, time_range)
    sys.exit(0 if success else 1)

if __name__ == "__main__":
    main()
