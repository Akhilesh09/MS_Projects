#!/usr/bin/env python3

import os
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import axes3d

def format_plot_labels(labels):
    """
    Beat plot labels into submission.

    Parameters
    ----------
    labels : list
        List of plot labels to be formatted.

    Returns
    -------
    List/array/whatever of formatted labels.

    """
    
    # Determine if labels are numbers or string identifiers
        
    try:
        labels = np.asarray(labels, dtype=float)
        numeric = True
    except ValueError:
        numeric = False
    
    if not numeric:
        return labels # No formatting needed
    
    else:
        
        logspace = False
        # Determine if values are logarithmically spaced
        labels_sorted = np.sort(labels[labels != 0.0])
        
        for i in range(labels_sorted.size-1):
            if labels_sorted[i+1] / labels_sorted[i] > 5.0:
                logspace = True
                break
        
        # Finally, convert to formatted string
        labels_str = []
        
        if logspace:
            for i in range(labels.size):
                labels_str.append(np.format_float_scientific(labels[i],
                                                             precision=2,
                                                             trim='-'))
        
        else:
            for i in range(len(labels)):
                try:
                    labels_str.append(np.format_float_positional(labels[i],
                                                                 precision=2,
                                                                 trim='-',
                                                                 fractional=False))
                except TypeError:
                    labels_str.append(str(labels[i]))
        
        return np.asarray(labels_str)

def grid_search_cv_plot(DF,filename=None):
    """
    Shows 2 best parameters vs accuracy plot for the model.
    
    Parameters
    ----------
    params: list
        List of best parameter-value pairs.
    
    combinations: list
        List of possible parameter combinations.
    
    accuracies: list
        List of possible parameter combination-accuracy pairs.
    
    """
    
    fpl = format_plot_labels # Shortcut function name
    
    # Get names of varied parameters
    plot_axes=[x.split('param_')[-1] for x in DF.columns if "param_" in x]
    
    ndim = len(plot_axes)
    assert ndim <= 3
    
    # Get all the unique values for each component of the points
    values_unique = {name : np.unique(DF['param_'+name]) for name in plot_axes}
    # Sort x, y, z axis by number of unique values for aesthetic purposes
    axes_names, axes_sizes = zip(*values_unique.items())
    axes_sizes = [x.size for x in axes_sizes]
    ind_sort = np.argsort(axes_sizes)[::-1]
    plot_axes = np.array(plot_axes)[ind_sort]
    
    # Create mapping to map parameter values (strings) to numerical values
    mapping = {}
    for name, array in values_unique.items():
        
        ind = np.argsort(array)
        array = array[ind]
        
        mapping[name] = {array[i] : i for i in range(len(array))}
    
    accs = np.array(DF['mean_test_score'])
    errs = np.array(DF["confidence"])
    
    err_upper = accs + errs
    err_upper[err_upper > 1.0] = 1.0
    err_lower = accs - errs
    err_lower[err_lower < 0.0] = 0.0
    
    # Identify statistically equivalent points with highest accuracy
    # Find point with highest upper bound and largest confidence interval
    lower_bound = err_lower[err_upper == err_upper.max()].min()
    best_ind = np.argwhere(err_upper >= lower_bound)
    other_ind = np.argwhere(err_upper < lower_bound)
    
    fig = plt.figure()
    ax = fig.add_subplot(111) if ndim == 1 else plt.axes(projection='3d')
    ax.set_title("Accuracy Range: [{:.4f}, {:.4f}]".format(err_lower.min(),
                                                           err_upper.max()))
    
    err_upper -= accs
    err_lower = accs - err_lower
    
    if ndim == 1:
    
        ax.set_xlabel(plot_axes[0])
        ax.set_ylabel("Accuracy")
        
        xs = DF['param_'+plot_axes[0]]
        
        try:
            xs = np.asarray(xs, dtype=float)
            ax.errorbar(xs[best_ind], accs[best_ind],
                        yerr=[err_lower[best_ind], err_upper[best_ind]],
                        capsize=2, color='blue', fmt='o')
            ax.errorbar(xs[other_ind], accs[other_ind],
                        yerr=[err_lower[other_ind], err_upper[other_ind]],
                        capsize=2, color='black', fmt='o')
              
        except:
            xs = [mapping[plot_axes[0]][x] for x in xs]
            
            ax.errorbar(xs[best_ind], accs[best_ind],
                        yerr=[err_lower[best_ind], err_upper[best_ind]],
                        capsize=2, color='blue', fmt='o')
            ax.errorbar(xs[other_ind], accs[other_ind],
                        yerr=[err_lower[other_ind], err_upper[other_ind]],
                        capsize=2, color='black', fmt='o')
            
            ax.set_xticks(list(mapping[plot_axes[0]].values()))
            ax.set_xticklabels(list(mapping[plot_axes[0]].keys()))
            
        
    
    elif ndim == 2:
        
        ax.set_xlabel(plot_axes[0])
        ax.set_ylabel(plot_axes[1])
        ax.set_zlabel("Accuracy")
        
        xs = DF['param_'+plot_axes[0]]
        ys = DF['param_'+plot_axes[1]]
        
        xs = [mapping[plot_axes[0]][x] for x in xs]
        ys = [mapping[plot_axes[1]][y] for y in ys]
            
        for i in range(accs.size):
            color = "blue" if i in best_ind else "black"
            acc = accs[i]
            x = xs[i]; y = ys[i]; low_w = acc - err_lower[i]; high_w = acc + err_upper[i]
            ax.scatter(x, y, acc, marker='o', s=2, color=color)
            ax.plot([x,x], [y,y], [low_w, high_w], marker="_", color=color)
        
        ax.set_xticks(list(mapping[plot_axes[0]].values()))
        ax.set_xticklabels(fpl(list(mapping[plot_axes[0]].keys())))
        ax.set_yticks(list(mapping[plot_axes[1]].values()))
        ax.set_yticklabels(fpl(list(mapping[plot_axes[1]].keys())))
    
    elif ndim == 3:
        
        ax.set_xlabel(plot_axes[0])
        ax.set_ylabel(plot_axes[1])
        ax.set_zlabel(plot_axes[2])
        
        xs = DF['param_'+plot_axes[0]]
        ys = DF['param_'+plot_axes[1]]
        zs = DF['param_'+plot_axes[2]]
        
        # Set up point sized based on accuracy
        min_size, max_size = 10, 100 # point size bounds
        # 100%/500 = 0.2% confidence interval will appear as point twice the size of
        # data point
        err_scale = 500
        # Size each point based on accuracy
        sizes = np.array(DF['mean_test_score'])
        errs = 1.0 + err_scale * np.array(errs) / sizes
        # Set sizes between min_size, max_size
        sizes *= (max_size - min_size) / (max(sizes) - min(sizes))
        sizes += min_size - min(sizes)
        errs *= sizes
        
        ss = sizes
        es = errs
        
        i = 0
        for x, y, z, s, e in zip(xs, ys, zs, ss, es):
            col = 'blue' if i in best_ind else 'black'
            x_map = mapping[plot_axes[0]][x]
            y_map = mapping[plot_axes[1]][y]  
            z_map = mapping[plot_axes[2]][z]  
            
            # Plot error
            ax.scatter(x_map, y_map, z_map, s=e, c='red')
            # Plot accuracy
            ax.scatter(x_map, y_map, z_map, s=s,c=col)
            
            i += 1
        
        ax.set_xticks(list(mapping[plot_axes[0]].values()))
        ax.set_yticks(list(mapping[plot_axes[1]].values()))
        ax.set_zticks(list(mapping[plot_axes[2]].values()))
        
        ax.set_xticklabels(fpl(list(mapping[plot_axes[0]].keys())))
        ax.set_yticklabels(fpl(list(mapping[plot_axes[1]].keys())))
        ax.set_zticklabels(fpl(list(mapping[plot_axes[2]].keys())))
    
    plt.tight_layout()
    
    if filename is not None:
        # Make sure directory exists
        file_dir = filename[:filename.rfind("/")]
        if "/" in filename and not os.path.isdir(file_dir):
            os.makedirs(file_dir)
        fig.savefig(filename, bbox_inches="tight")
    else:
        plt.show()