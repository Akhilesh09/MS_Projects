#!/usr/bin/env python3

import os, sys, csv, time
import copy
import numpy as np
import pandas as pd
from sklearn.feature_selection import VarianceThreshold
from sklearn.preprocessing import StandardScaler, RobustScaler
from sklearn.preprocessing import scale

class DataParser:
    """
    This class parses and stores data into a Pandas DataFrame and features
    several routines for printing data to files in various formats.

    The dataset must be within a main directory containing subdirectories, all
    of which contain a file containing the collected data (features) and their
    values. The subdirectories should be named uniquely named and the data
    files should all be of the same name. Within the dataset, there must exist
    a file specifying the features to collect and the programs to parse them
    from.
    """

    def __init__(self):
        """ Class initializer.
        """

        # Boolean for if data has been parsed
        self._parsed = False

        # Member attributes
        self._features = None
        self._programs = None
        self._stamps = None
        self._data = pd.DataFrame()

    # ====================> Private functions

    def _parse_features_list(self, features_path):
        """
        Parse through the file containing the list of features and
        store uniquely named features within a list.
        """

        # Init list to store features
        self._features = []

        # Open feature names file
        with open(features_path, "r") as features:
            
            # Loop through features
            for line in features:

                # Get feature from line
                feature = line.strip().split()[0]
                
                # If unique, add to list
                if feature not in self._features:
                    self._features.append(feature)
        return self._features


    def _parse_programs_list(self, programs_path, data_file=None):
        """
        Parse through the file containing the list of program names,
        check that the program directory exists and has a file within
        it of the same name as the provided ``data_file'' and store
        the uniquely named programs that meet this criteria within a list.
        """

        # Init unique full program names
        self._programs = []
        
        # Open programs file
        data_dir = programs_path[:programs_path.rfind("/")]
        with open(programs_path, "r") as programs:

            # Loop through base program names
            for line in programs:

                if data_file is not None:
                    # Get base program name from line
                    base = []
                    tmp = line.split("-")
                    for entry in tmp:
                        if not entry.isdigit():
                            base.append(entry)
                        else:
                            base = "-".join(base)
                            break
                    
                    # Loop through contents of rootdir
                    for program in os.listdir(data_dir):

                        # Check whether this program corresponds to this entry
                        if base in program:
                        
                            # Check if entry exists in full program name list
                            if program not in self._programs:

                                # Form path to data file
                                path = os.path.join(
                                    data_dir, program, data_file
                                )
                                # If the data file exists for this entry
                                if os.path.isfile(path):
                                    self._programs.append(program)
                else:
                    self._programs.append(line)
        return self._programs

    def _parse_stamps(self):
        """
        Parse the unique stamps from all programs.
        """

        # Iterate through stamps and get unique
        unique_stamps = []
        for program in self._programs:
            tmp = program.split("-")
            for entry in tmp:
                if entry.isdigit():
                    if entry not in unique_stamps:
                        unique_stamps.append(entry)
        return unique_stamps

    # ====================> Properties

    @property
    def features(self):
        """
        Return the data frame holding the feature 
        names and number of occurences.
        """
        if self._parsed:
            return self._features
        else:
            msg = "Data has not ye been parsed.\n"
            print(msg)
            return None
    
    @property
    def n_features(self):
        """
        Return the number of unique features.
        """
        if self._parsed:
            return len(self._features)
        else:
            msg = "Data has not ye been parsed.\n"
            print(msg)
            return None

    @property
    def programs(self):
        """
        Return the data frame holding the program names and
        number of occurences.
        """
        if self._parsed:
            return self._programs
        else:
            msg = "Data has not ye been parsed.\n"
            print(msg)
            return None

    @property
    def n_programs(self):
        """
        Return the number of unique programs.
        """
        if self._parsed:
            return len(self._programs)
        else:
            msg = "Data has not ye been parsed.\n"
            print(msg)
            return None

    @property
    def data(self):
        """
        Return the data frame containing the parsed data
        """
        if self._parsed:
            return self._data
        else:
            msg = "Data has not ye been parsed.\n"
            print(msg)
            return None

    # ====================> Public functions

    def parse_dataset(self, features_path="../raw_data/newdata/features_573.txt", 
                      programs_path="../raw_data/newdata/programsnames.txt", 
                      data_file="stats.txt", 
                      NAN=0.001, VAR_THRESH=1e-10, verbosity=1):
        """
        Parse through the data files within each program by line, find the
        desired features, and store their values (or a default value in the
        case of missing values) within a Pandas dataframe with the feature
        names as the row labels and program names as the column labels.

        Params
        ------
        NAN : Value to set unencountered values to.
        THRESH : Threshold for variance cutoff
        verbosity : Level of screen output.
        """
        tic = time.perf_counter()

        self._parsed = True
        if verbosity > 0:
            print("\n"+">"*5 + " Parsing data...")

        # Get data directory
        data_dir = programs_path[:programs_path.rfind("/")]

        # Parse program and feature names
        self._programs = self._parse_programs_list(programs_path, data_file)
        self._features = self._parse_features_list(features_path)
        
        # Loop through program names
        for program in self._programs:
            if verbosity > 1:
                print("Parsing data for {}".format(program))

            # Form path to data-file
            path = os.path.join(data_dir, program, data_file)

            # Go through stats file
            with open(path, "r") as stats:

                # Init features dictionary for this program
                program_features = {}

                # Loop through lines of stats file
                for line in stats:
                    
                    # Skip empty lines
                    if line.isspace():
                        continue
                    
                    # Split line by white-space
                    stat = line.strip().split()

                    # Add feature, if the feature exists in the file
                    if stat[0] in self._features:
                        try:
                            val = int(stat[1])
                            program_features[stat[0]] = val
                        except ValueError:
                            try:
                                val = float(stat[1])
                                program_features[stat[0]] = val
                            except ValueError:
                                continue

                # Make dictionary into DataFrame and add to main
                program_DF = pd.DataFrame.from_dict(
                    program_features, orient="index", columns=[program]
                )
                self._data = self._data.append(program_DF.T, sort=True)
    
        # Fill missing values
        self._data.fillna(NAN, inplace=True)

        # Filter out low variance features
        VT = VarianceThreshold(VAR_THRESH)
        VT.fit(self._data)
        self._data = self._data.iloc[:,VT.get_support(indices=True)]

        toc = time.perf_counter()
        if verbosity > 0:
            print("\n" + ">"*5 + " Parsing data took {:.3f} seconds.\n".format(toc-tic))
        return self.data

    def parse_datafile(self, data_path="../parsed_data/allstats_941_486.txt", 
                       programs_path="../parsed_data/programsnames_941.txt",
                       verbosity=0):
        """
        Parse a pre-parsed data file.
        """
        tic = time.perf_counter_ns()

        # Set parsed to True
        self._parsed = True
        if verbosity > 0:
            print("\n"+">"*5 + " Parsing data...")

        # Get program names
        self._programs = self._parse_programs_list(programs_path)
        # Parse the data-set
        data = pd.read_csv(
            data_path, header=None, index_col=0, names=self._programs)
        self._data = data.T

        toc = time.perf_counter_ns()
        if verbosity > 0:
            print("\n" + ">"*5 + " Parsing data took {:.3f} seconds.\n".format(toc-tic))
        return self.data

    def write_allstats(self, filename, norm_opt="", domain_opt ="global"):
        """
        Write the data to filename after applying norm_opt normalization.

        Params
        ------
        norm_opt: Normalization option.
        domain_opt  : Normalization domain
        filename : Filename to save output to.
        """
        if self._parsed:
            msg = ">"*5 + " Writhing data to {}"
            print(msg.format(filename))

            # Normalize according to NORM
            data = self.preprocess_data(norm_opt, domain_opt ).T

            # Write to file
            data.to_csv(
                filename, sep=",", index=True, header=False,
                float_format="%.8f"
            )
        else:
            msg = "Data has not ye been parsed.\n"
            print(msg)
            return None

    def write_program_names(self):
        """
        Write program names to new file.
        """
        filename = "programsnames_" + str(self._data.shape[1])  + ".txt"
        # Open file for writing
        with open(filename, "w") as file:
            for p, program in enumerate(self._data.columns):
                file.write(program)
                if p < len(self._data.columns)-1:
                    file.write("\n")

########################
##### OLD ROUTINES #####
########################

    def preprocess_data(self, norm_opt="", domain_opt="global"):
        """
        Normalize the data from the parsed dataset to the maximum value of
        each type of data collected.

        Params
        ------
        norm_opt : str
            Normalization option.
                "" = No normalization
                max = maximum value normalization
                standard = remove mean and divide by standard deviation
                robust = remove median and divide by inter-quartile range
        domain_opt : str
            Domain option to apply normalization to.
                global = Full domain
                time = For each time snapshot.
        
        Returns
        -------
        Data normalized to norm_opt and domain_opt
        """
        if self._parsed:

            # Return data for no normalization
            if norm_opt== "":
                return self.data
        
            # For global options
            if domain_opt  == "global":
                # Divide by max
                if norm_opt== "max":
                    return self._max_normalization(self.data)

                # Remove mean, divide by std
                elif norm_opt== "standard":
                    return self._standard_scaler(self.data)

                # Remove median, divide by iqr
                elif norm_opt== "robust":
                    return self._robust_scaler(self.data)

                # Value error
                else:
                    msg = "{} normalization not implemented."
                    raise ValueError(msg.format(norm_opt))

            elif domain_opt  == "time":
                # Divide by max
                if norm_opt== "max":
                    return self._time_normalization(method=0)
                
                # Remove mean, divide by std
                elif norm_opt== "standard":
                    return self._time_normalization(method=1)
                
                # Remove median, divide by iqr
                elif norm_opt== "robust":
                    return self._time_normalization(method=2)

                # Value error
                else:
                    msg = "{} normalization not implemented."
                    raise ValueError(msg.format(norm_opt))

            else:
                msg = "{} option not implemented."
                raise ValueError(msg.format(domain_opt))
        else:
            msg = "Data has not ye been parsed.\n"
            print(msg)
            return None

    @staticmethod
    def _max_normalization(X):
        """
        Perform a normalization to the maximum observed values
        of the features from the provided DataFrame.
        """
        # To avoid zero division for a feature with 
        # a constant zero value
        max_value = X.max().replace(to_replace=0, value=1)
        return X.div(max_value, axis=1)
        
    @staticmethod
    def _standard_scaler(X):
        """
        Remove the mean and normalize by the standard deviation
        of the dataset.
        """
        scaler = StandardScaler()
        return pd.DataFrame(
            scaler.fit_transform(X), 
            index=X.index, columns=X.columns
        )
    
    @staticmethod
    def _robust_scaler(X):
        """
        Remove the median and divide by the inter-quartile range
        of the data-set.
        """
        scaler = RobustScaler()
        return pd.DataFrame(
            scaler.fit_transform(X), 
            index=X.index, columns=X.columns
        )

    def _time_normalization(self, method=0):
        """
        Normalize features based on similar time stamps.
        """
        # Get unique time stamps
        stamps = self._parse_stamps()
        
        data = copy.deepcopy(self.data)
        for stamp in stamps:
            progs = []
            for program in self._programs:
                tmp = program.split("-")
                for entry in tmp:
                    if entry == stamp:
                        progs.append(program)
            if method == 0:
                data.loc[progs] = self._max_normalization(
                    data.loc[progs]
                )
            elif method == 1:
                data.loc[progs] = self._standard_scaler(
                    data.loc[progs]
                )
            elif method == 2:
                data.loc[progs] = self._robust_scaler(
                    data.loc[progs]
                )
        return data
