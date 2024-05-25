import os
import subprocess
import sys

import pandas as pd

pd.set_option("display.precision", 18)

directory_name = "csvs/"


def main(csv_file: list[str]):
    precision_threshold = 1e-10
    csv_files = (
        [
            directory_name + filename
            for filename in os.listdir(directory_name)
            if ".csv" in filename
        ]
        if csv_file == None
        else csv_file
    )

    for filename in csv_files:
        p = subprocess.run(["./csv_reader.out", filename, "1"])
        if p.returncode != 0:
            exit(1)
        print(f"CSV FILE: {filename}")
        compare_two_result(filename, "/tmp/" + filename[5:], precision_threshold)

        print("=================================================")


def compare_two_result(orig_csv, interpreted_csv, precision_threshold):
    csv1 = pd.read_csv(orig_csv)
    csv2 = pd.read_csv(interpreted_csv, header=None)

    features = list(csv1)

    for i in range(len(features)):
        df1 = csv1.loc[:, features[i]]
        df2 = csv2.loc[:, i]

        combined_view = {"orig": df1, "interpreted": df2}
        combined_view = pd.DataFrame(combined_view, index=df1.index)

        idx = (
            abs(combined_view["orig"] - combined_view["interpreted"])
            < precision_threshold
        )
        if idx.all() == False:
            print(f"For feature: {features[i]}, feature_num: {i}")
            print(combined_view[~idx])
            print("\n-------------------------------------------\n")


if __name__ == "__main__":
    if len(sys.argv) == 2:
        main([sys.argv[1]])
    else:
        main(None)
